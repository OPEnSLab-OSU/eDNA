#include "KPLogging.hpp"
#include "KPPowerManagement.hpp"
#include "KPPump.hpp"
#include "KPSDCard.hpp"
#include "KPSensorManager.hpp"
#include "KPSerialCommand.hpp"
#include "KPServer.hpp"
#include "KPShiftRegister.hpp"
#include "KPSystemEnvironment.hpp"
#include "KPTaskManager.hpp"

#define BitCount 32

KPSDCard card;
KPPump pump("pump", Motor_Forward_Pin, Motor_Reverse_Pin);
KPShiftRegister shift(BitCount, Shift_Register_Data_Pin, Shift_Register_Clock_Pin, Shift_Register_Latch_Pin);
KPServer web("web", "eDNA", "password");
KPSerialCommand serial_command;
KPPowerManagement power;

KPSensorManager sensor_manager;
KPLogging logging_system;

//===============================================
// [+_+] Shift Register devices
//===============================================
enum ShiftDevice { Air = 2,
                   Alcohol = 3,
                   Flush = 4 };

//===============================================
// [+_+] Stop KPState
//===============================================
class StopState : public KPState {
public:
    void update(KPStateMachine & machine) override {
        KPStatus & status = KPStatus::current();
        if (timeSinceLastTransition() >= 2) {
            pump.off();
            shift.writeZeros();
            shift.writeLatchOut();

            if (status.valveCurrent <= status.valveLowerBound) {
                status.valveCurrent = status.valveLowerBound;
                status.isFull = true;
            } else {
				status.valves[status.valveCurrent] = 0;
                status.valveCurrent--;
                // power.scheduleNextAlarm(status.samplingInteval);
            }

            machine.transitionTo(KPStateIdentifier::idle);
            Serial.println(F("Transitioning to Idle"));
        }
    }
};

//===============================================
// [+_+] Decontamination KPState
//===============================================
class DecontaminateState3 : public KPState {
public:
	int count = 0;

	void update(KPStateMachine & machine) override {
		if (isInitialTransition()) {
			shift.writeZeros();
			shift.writeLatchOut();
			shift.set(Alcohol, HIGH);
			shift.set(Flush, HIGH);
			shift.flush();
			pump.on();
			count++;
		} 
		
		// Alcohol intake for 3 seconds
		if (timeSinceLastTransition() >= 3) {
			shift.set(Alcohol, LOW);
			shift.flush();
			pump.on(Direction::reverse);
		}

		// Purge the system for 12 seconds
		if (timeSinceLastTransition() >= 15) {
			if (count == 2) {
				count = 0;
				machine.transitionTo(KPStateIdentifier::stop);
			} else {
				machine.transitionTo(KPStateIdentifier::decontaminate2);
			}
		}
	}
};

class DecontaminateState2 : public KPState {
public: 
	void update(KPStateMachine & machine) override {
		if (isInitialTransition()) {
			// Turn off all the valves except the Air intake and rise the pressure until 15 psi
			shift.setZeros();
			shift.set(Air, HIGH);
			shift.flush();
			pump.on();
		}

		if (KPStatus::current().pressure >= 15) {
			pump.off();
            shift.set(Flush, HIGH);
            shift.flush();
			delay(3000);

			machine.transitionTo(KPStateIdentifier::decontaminate3);
		}
	}
};


class DecontaminateState1 : public KPState {
public:

    void update(KPStateMachine & machine) override {
        auto status = KPStatus::current();
        if (isInitialTransition()) {
            // Turn everything off
            pump.off();
            shift.writeLatchOut();
            shift.setZeros();
            shift.flush();

            // Turn on the flush valve, the air intake, and the pump to clean out the main pipe
            shift.set(Flush, HIGH);
            shift.set(Air, HIGH);
            shift.flush();
            pump.on();
        }

        // After 10 seconds, get rid of the water between the filter and the main pipe
        if (timeSinceLastTransition() >= 10) {
            // Rise the pressure in the system to 1.7 psi
            pump.pwm(0.75);
            shift.set(Flush, LOW);
            shift.flush();

            if (status.pressure >= 1.7) {
                int r = shift.registerIndex(status.valveCurrent) + 1;
                int b = shift.bitIndex(status.valveCurrent);

                pump.off();
                shift.setRegister(r, b, HIGH);
                shift.flush();
				delay(50);

				machine.transitionTo(KPStateIdentifier::decontaminate2);
            }
        }
    }
};

//===============================================
// [+_+] Preserve KPState
//===============================================
class PreserveState : public KPState {
public:
    PreserveState() {
        timeLimit = 15;
        timeMax = MinsToSecs(1);
    }

    void update(KPStateMachine & machine) override {
        if (isInitialTransition()) {
            int r = shift.registerIndex(KPStatus::current().valveCurrent) + 1;
            int b = shift.bitIndex(KPStatus::current().valveCurrent);

            shift.writeLatchOut();
            shift.setZeros();
            shift.set(Alcohol, HIGH);
            shift.setRegister(r, b, HIGH);  // Filter
            shift.flush();
            pump.on();
        }

        if (timeSinceLastTransition() >= timeLimit) {
            machine.transitionTo(KPStateIdentifier::decontaminate1);
            Serial.println(F("Transitioning to Stop"));
        }
    }
};

//===============================================
// [+_+] Clean KPState
//===============================================
class DryState : public KPState {
public:
    void update(KPStateMachine & machine) override {
        KPStatus & status = KPStatus::current();
        if (isInitialTransition()) {
            int r = shift.registerIndex(status.valveCurrent) + 1;
            int b = shift.bitIndex(status.valveCurrent);

            shift.setZeros();
            shift.set(Air, HIGH);
            shift.setRegister(r, b, HIGH);  // Filter
            shift.flush();
            pump.on();
        }

        if (timeSinceLastTransition() >= 10) {
            machine.transitionTo(KPStateIdentifier::preserve);
            Serial.println(F("Transitioning to Preserve"));
        }
    }
};

//===============================================
// [+_+] Sample KPState
//===============================================
class SampleState : public KPState {
public:
    SampleState() {
        timeLimit = MinsToSecs(3);
        timeMax = MinsToSecs(20);
        pressureLimit = 8;
        pressureMax = 15;
        volumeLimit = 1000;
        volumeMax = 5000;
    }

    void update(KPStateMachine & machine) override {
        KPStatus status = KPStatus::current();
        if (isInitialTransition()) {
            int r = shift.registerIndex(status.valveCurrent) + 1;
            int b = shift.bitIndex(status.valveCurrent);

            shift.setZeros();
            shift.writeLatchIn();
            shift.setRegister(r, b, HIGH);  // Filter
            shift.flush();
            pump.on();
        }

        // Transition to clean when the pressure exceeds 8 PSI or when exceeds 15 seconds
        if (timeSinceLastTransition() >= timeLimit || status.pressure >= pressureLimit) {
            machine.transitionTo(KPStateIdentifier::dry);
            shift.writeLatchOut();
            Serial.println(F("Transitioning to Clean"));
        }
    }
};

//===============================================
// [+_+] Flush KPState
//===============================================
class FlushState : public KPState {
public:
    FlushState() {
        timeLimit = 120;
        timeMax = MinsToSecs(7);
        volumeLimit = 1000;
        volumeMax = 5000;
    }

    void update(KPStateMachine & machine) override {
        if (isInitialTransition()) {
			shift.setZeros();				// Reset TPIC values
            shift.writeLatchIn();
            shift.set(Flush, HIGH);
            shift.flush();
			pump.on();
        }

        if (timeSinceLastTransition() >= timeLimit) {
            machine.transitionTo(KPStateIdentifier::sample);
            Serial.println(F("Transitioning to Sample"));
        }
    }
};

//===============================================
// [+_+] HTTP Routings
//===============================================
void setupWebServerHPPTRoutings() {
    // GET | Homepage
    web.get("/", [](Request & req, Response & res) {
        res.contentType = "text/html";
        res.serveContentFromFile("index.htm", card);
        res.end();
        printFreeRam();
    });

    // GET | Reponse with current status of the system in JSON
    web.get("/status", [](Request & req, Response & res) {
        res.contentType = "application/json";
        KPStatus & status = KPStatus::current();

        const int size = 1000;
        StaticJsonDocument<size> doc;

        using namespace JsonKey;
        doc[stateName] 				= app.getCurrentStateName();
        doc[stateId] 				= static_cast<int>(app.getCurrentStateId());
        doc[valveBegin] 			= status.valveBegin;
        doc[valveLowerBound] 		= status.valveLowerBound;
        doc[valveUpperBound] 		= status.valveUpperBound;
        doc[valveCurrent] 			= status.valveCurrent;
        doc[pressure] 				= status.pressure;
        doc[temperature] 			= status.temperature;
        doc[waterFlow] 				= status.waterFlow;
		doc["barometric"]			= status.barometric;
		doc[waterDepth]				= status.waterDepth;
		doc[timeUTC]				= now();

        JsonArray valveArray = doc.createNestedArray(valves);
        copyArray(status.valves, valveArray);

        char buffer[size];
        serializeJson(doc, buffer);

        res.send(buffer);
        res.end();

        serializeJsonPretty(doc, Serial);
        printFreeRam();
    });

    // POST | Select and transition to the given state name specified in the JSON document
    web.post("/select-state", [](Request & req, Response & res) {
        StaticJsonDocument<300> doc;
        deserializeJson(doc, req.body);
        Serial.println();
		res.contentType = "text/plain";
        // serializeJsonPretty(doc, Serial);

        const char * name = doc["name"];
        for (int i = 0; i < app.statesCount(); i++) {
            KPState & state = app.getStateForIndex(i);
            if (strcmp(state.name, name) == 0) {
                app.transitionTo(state.id);
				println("Transitioning to", name, "state");
				res.send("success");
                res.end();
                return;
            }
        }

        res.send("Not Found");
        res.end();
    });

    web.post("/setTime", [](Request & req, Response & res) {
        StaticJsonDocument<JSON_OBJECT_SIZE(5)> doc;
        deserializeJson(doc, req.body);
        ulong utcTime = doc[JsonKey::timeUTC];
        Serial.println(utcTime);
        power.set(utcTime);
        res.end();
    });

    web.post("/saveTask", [](Request & req, Response & res) {
        StaticJsonDocument<TaskDocumentSize> doc;
        deserializeJson(doc, req.body);

        JsonObject taskJson = doc.as<JsonObject>();
        KPTask task(taskJson);

        // Check if task configuration is valid
        if (!task.validate()) {
            res.send("Failed: Invalid configuration");
            res.end();
            return;
        }

        // Check against current scheduled tasks
        StaticJsonDocument<TaskRefSize * 24> taskrefs;
        app.loadJsonFromStorage("active", taskrefs);
        for (JsonVariant v : taskrefs.as<JsonArray>()) {
            if (v["id"] == task.id) {
                res.send("Task is on schedule, please stop the task before making changes");
                return;
            }
        }

        // Replace the task file in the SD Card
        app.updateTaskFile(task);
        res.end();
    });

    web.post("/stopTask", [](Request & req, Response & res) {
		res.contentType = "application/json";
        StaticJsonDocument<TaskDocumentSize> doc;
        deserializeJson(doc, req.body);
        JsonObject taskJson = doc.as<JsonObject>();
        if (app.currentTask.validate() && taskJson["id"] == app.currentTask.id && app.getCurrentStateId() != KPStateIdentifier::idle) {
            app.transitionTo(KPStateIdentifier::stop);
			app.currentTask.next();
        }
		
		app.markTaskUnactive(app.currentTask.id);
		res.send("{}");
		res.end();
    });

	web.post("/getTask", [](Request & req, Response & res) {
		StaticJsonDocument<100> doc;
		deserializeJson(doc, req.body);

		const int id = doc["id"];

		StaticJsonDocument<TaskDocumentSize> taskdoc;
		app.loadTaskFromStorage(id, taskdoc);

		char buffer[taskdoc.memoryUsage()];
		serializeJson(taskdoc, buffer, taskdoc.memoryUsage());

		println(taskdoc.memoryUsage());

		res.contentType = "application/json";
		res.send(buffer);
		res.end();

		serializeJsonPretty(taskdoc, Serial);
	});

    web.get("/taskrefs", [](Request & req, Response & res) {
        StaticJsonDocument<TaskRefSize * 24> taskrefs;
        app.loadTaskRefsFromStorage(taskrefs);

        char buffer[taskrefs.memoryUsage()];
		Serial.println(taskrefs.memoryUsage());
        serializeJson(taskrefs, buffer, taskrefs.memoryUsage());
		res.contentType = "application/json";
        res.send(buffer);
        res.end();

        serializeJsonPretty(taskrefs, Serial);
    });

    web.get("/update-rtc", [](Request & req, Response & res) {
        StaticJsonDocument<100> doc;
        deserializeJson(doc, req.body);
        ulong utc = doc["utc"];
        // Reasonable utc time
        if (utc >= 155555555) {
            power.set(utc);
        } else {
            res.send("UTC time is not reasonable");
        }

        res.end();
    });
}

//===============================================
// [+_+] Status Report
//===============================================
void printStatusReport() {
    auto & s = KPStatus::current();
    Serial.print("Current Time: ");
    Serial.println((long) power.timeKeeper.get());
    Serial.print("Pressure: ");
    Serial.println(s.pressure);
    Serial.print("Temperature: ");
    Serial.println(s.temperature);
    Serial.print("Water flow: ");
    Serial.println(s.waterFlow);
	println("Barometric: ", s .barometric);

    auto time = app.getStateForIdentifier(KPStateIdentifier::sample).timeLimit;
    Serial.print("Sample-state time limit: ");
    Serial.println(time);
    Serial.println();
}

//===============================================
// [+_+] Shutdown: make sure that everthing is off
//===============================================
void shutdown() {
    app.scheduler.removeAllActions();
	pump.off();
    shift.writeZeros();
    shift.writeLatchOut();
    power.shutdown();
}

//===============================================
// [+_+] Prepare: This is where components should be added
//===============================================
void prepare() {
	// delay(10000);
	Serial.begin(9600);
	Wire.begin();

	// RTC Interrupt Pin and Override Pin are both active low
	// This checks for when the power is no intterupt from neither the rtc or override button
	// if (analogRead(RTC_Interrupt_Pin) >= 100 && analogRead(Override_Mode_Pin) >= 100) {
	// 	shutdown();
	// }

    // [+_+] State Registrations
    app.registerState<IdleState>(KPStateIdentifier::idle, "idle");
    app.registerState<StopState>(KPStateIdentifier::stop, "stop");
    app.registerState<FlushState>(KPStateIdentifier::flush, "flush");
    app.registerState<SampleState>(KPStateIdentifier::sample, "sample");
    app.registerState<DryState>(KPStateIdentifier::dry, "dry");
    app.registerState<PreserveState>(KPStateIdentifier::preserve, "preserve");
    app.registerState<DecontaminateState1>(KPStateIdentifier::decontaminate1, "decontaminate1");
    app.registerState<DecontaminateState2>(KPStateIdentifier::decontaminate2, "decontaminate2");
	app.registerState<DecontaminateState3>(KPStateIdentifier::decontaminate3, "decontaminate3");

    // [+_+] Notifications Observe
    KPNotificationCenter::shared().observe(Event::SCCommandEntered, [](void * arg) {
        String * rawPtr = reinterpret_cast<String *>(arg);
        String input = *rawPtr;
        Serial.println(input);

		if (input == "status") {
			KPSequentialAction s("status");
			s.wait(1000, printStatusReport);
			runForever(s);
		} else if (input == "stop status") {
			app.scheduler.removeAction("status");
		} else if (input == "reverse") {
			shift.setZeros();
			shift.writeLatchOut();
			shift.set(Flush, HIGH);
			shift.flush();
			pump.on(Direction::reverse);
		} else if (input == "off") {
			shutdown();
		} else if (input == "json") {
			StaticJsonDocument<100> doc;
			deserializeJson(doc, "{id: 1000}");
			serializeJson(doc, Serial);

			deserializeJson(doc, "{id: 2000}");
			serializeJson(doc, Serial);
		}
    });

    KPNotificationCenter::shared().observe(Event::TMTaskLoaded, [](void * arg) {
        // JsonObject * rawPtr = reinterpret_cast<JsonObject *>(arg);
        // JsonObject task = *rawPtr;
    });

    // KPNotificationCenter::shared().observe(Event::LGFinishedLogging, [](void * arg) {
    //     auto & status = KPStatus::current();
    //     if (status.isProgrammingMode == false && app.getCurrentStateId() == KPStateIdentifier::idle) {
    //         power.shutdown();
    //     }
    // });

    setupWebServerHPPTRoutings();

    addComponent(web);
    addComponent(card);
    addComponent(pump);
    addComponent(shift);
    addComponent(power);
    addComponent(serial_command);
    addComponent(sensor_manager);
    addComponent(logging_system);

    app.transitionTo(KPStateIdentifier::idle);
}

//===============================================
// [+_+] Ready: Called after setup method on each component has been executed
//===============================================
void ready() {
    if (web.ready) {
        web.printWiFiStatus();
        web.begin();
    }

    if (card.ready) {
		app.updateSessionPath();
        card.loadConfigurationFromCurrentSession();
    }
}

//===============================================
// [+_+] Update: Analogous to Arduino loop()
//===============================================
void update() {
	println(app.getCurrentStateName(), KPStatus::current().isProgrammingMode());
	if (app.getCurrentStateId() == KPStateIdentifier::idle && KPStatus::current().isProgrammingMode() == false) {
		shutdown();
	}
}

KPSystem app(prepare, ready, update, card);

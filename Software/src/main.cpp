#include "ArduinoJson.h"
#include "OPPower.hpp"
#include "OPPump.hpp"
#include "OPSerialBuffer.hpp"
#include "OPServer.hpp"
#include "OPShiftRegister.hpp"
#include "OPSDCard.hpp"
#include "OPSystemEnvironment.hpp"
#include "SSC.h"

// Water 	0, 0
// Air 		0, 1
// Alcohol 	0, 2
// Flush 	0, 3

OPServer web("web", "eDNA", "password");
OPShiftRegister shift("shift", 32, 12, 11, 9); // [5, 9, 11, Old Board], [5, 9, 10, For LED TPICs]
OPPump pump("pump", 12, 13);
OPPower power("power");
OPSDCard card("card");
OPSerialBuffer serialBuffer("serial-buffer");
SSC pressureSensor(0x08);

bool psEnable = false;
enum class StateIdentifier {flush, sample, clean, preserve, stop, idle};

class StopState : public State {
public:
	void update(OPSystem & system) {
		Status & status = Status::current();
		if (system.isInitialTransition()) {
			pump.off();
		}

		if (system.timeSinceLastTransition() >= 2000) {
			shift.setZeros();
			shift.flush();

			if (status.currentValve >= status.endValve) {
				status.currentValve = status.endValve + 1;
				status.isFull = true;
			} else {
				status.currentValve++;
				power.scheduleNextAlarm(status.samplingInteval);
			}

			system.transitionTo(StateIdentifier::idle);
			Serial.println(F("Transitioning to Idle"));
		}
	}
};

class PreserveState : public State {
public:
	void update(OPSystem & system) {
		if (system.isInitialTransition()) {
			int r = shift.registerIndex(Status::current().currentValve) + 1;
			int b = shift.bitIndex(Status::current().currentValve);

			shift.setZeros();               // Reset TPIC Values
			shift.setRegister(0, 2, HIGH);  // Alcohol Intake
			shift.setRegister(r, b, HIGH);  // Filter
			shift.flush();
			pump.on();
		}

		// Transitioning Condition
		if (system.timeSinceLastTransition() >= 7000) {
			system.transitionTo(StateIdentifier::stop);
			Serial.println(F("Transitioning to Stop"));
		}
	}
};

class CleanState : public State {
public:
	void update(OPSystem  & system) {
		Status & status = Status::current();
		if (system.isInitialTransition()) {
			int r = shift.registerIndex(status.currentValve) + 1;
			int b = shift.bitIndex(status.currentValve);

			shift.setZeros();               // Reset TPIC Values
			shift.setRegister(0, 1, HIGH);  // Air Intake
			shift.setRegister(r, b, HIGH);  // Filter
			shift.flush();
			pump.on();
		}

		if (system.timeSinceLastTransition() >= 10000) {
			system.transitionTo(StateIdentifier::preserve);
			Serial.println(F("Transitioning to Preserve"));
		}
	}
};

class SampleState: public State {
public:
	
	unsigned long timeLimit = 1500000;
	int pressureLimit = 8;

	void update(OPSystem & system) {
		Status status = Status::current();
		if (system.isInitialTransition()) {
			int r = shift.registerIndex(status.currentValve) + 1;
			int b = shift.bitIndex(status.currentValve);

			shift.setZeros();               // Reset TPIC Values
			shift.setRegister(0, 0, HIGH);  // Water Intake
			shift.setRegister(r, b, HIGH);  // Filter
			shift.flush();
			pump.on();
		}

		// Transition to clean when the pressure exceeds 8 PSI or when exceeds 15 seconds
		if (system.timeSinceLastTransition() >= timeLimit || pressureSensor.pressure() >= pressureLimit) {
			system.transitionTo(StateIdentifier::clean);
			Serial.println(F("Transitioning to Clean"));
		}
	}

	void loadFrom(JsonDocument & doc) override {
		JsonObject obj = doc[name];
		int timeLimit = obj[JsonKey::JSON_TIME_LIMIT];
		int pressureLimit = obj[JsonKey::JSON_PRESSURE_LIMIT];
		Serial.println(timeLimit);
		Serial.println(pressureLimit);
	}

	void saveTo(JsonDocument & doc) override {
		JsonObject obj = doc.createNestedObject(name);
		obj[JsonKey::JSON_TIME_LIMIT] = timeLimit;
		obj[JsonKey::JSON_PRESSURE_LIMIT] = pressureLimit;
	}
};

class FlushState: public State {
public:
	void update(OPSystem & system) {
		if (system.isInitialTransition()) {
			shift.setZeros();               // Reset TPIC Values
			shift.setRegister(0, 0, HIGH);  // Water Intake
			shift.setRegister(0, 3, HIGH);  // Flush Valve
			shift.flush();
			pump.on();
		}

		if (system.timeSinceLastTransition() >= 10000) {
			system.transitionTo(StateIdentifier::sample);
			Serial.println(F("Transitioning to Sample"));
		}
	}
};

void prepare() {
	// <==> Serial <==>
	Serial.begin(9600); delay(5000);

	// <==> States <==>
	app.registerState<FlushState>(StateIdentifier::flush, "flush-state");
	app.registerState<SampleState>(StateIdentifier::sample, "sample-state");
	app.registerState<CleanState>(StateIdentifier::clean, "clean-state");
	app.registerState<PreserveState>(StateIdentifier::preserve, "preserve-state");
	app.registerState<StopState>(StateIdentifier::stop, "stop-state");
	app.registerState<IdleState>(StateIdentifier::idle, "idle-state");

	// <==> Web: HTTP Routering <==>
	web.get("/", [](Request & req, Response & res) {
		int limit = 0;
		const int size = 4096;
		char homepage[size]{0};
		while (card.loadContentsOfFile("index.htm", homepage, size) && limit < 20) {
			res.send(homepage);
			limit++;
		}

		res.end();
		printFreeRam();
	});

	// web.get("/", [](Request & req, Response & res) {
	// 	if (web.homepage == nullptr) {
	// 		web.homepage = card.loadContentsOfFile("index.htm");
	// 	}
	
	// 	res.send(web.homepage);
	// 	res.end();
	// 	printFreeRam();
	// });

	web.get("/status", [](Request & req, Response & res) {
		Status & status = Status::current();
		res.contentType = "application/json";

		const size_t size = JSON_OBJECT_SIZE(11);
		StaticJsonDocument<size> doc;
		doc["state-name"] = app.getCurrentStateName();
		doc["state-id"] = static_cast<int>(app.getCurrentState()->state->id);
		doc["valve-begin"] = status.startValve;
		doc["valve-current"] = status.currentValve;
		doc["valve-end"] = status.endValve;
		doc["valve-max"] = status.maxValve;
		doc["pressure"] = psEnable ? pressureSensor.pressure() : 0;
		doc["temperature"] = psEnable ? pressureSensor.temperature() : 0;
		doc["routine-interval"] = status.samplingInteval;
		char output[size];
		serializeJson(doc, output);
		res.send(output);
		res.end();
	});

	web.get("/flush", [](Request & req, Response & res) {
		res.contentType = "text/plain";
		res.send("success");
		res.end();
		app.transitionTo(StateIdentifier::flush);
		Serial.println("Transitioning to Flushing State");
	});

	web.get("/sample",[](Request & req, Response & res) {
		res.contentType = "text/plain";
		res.send("success");
		res.end();
		app.transitionTo(StateIdentifier::sample);
		Serial.println("Transitioning to Sampling State");
	});

	web.post("/sample", [](Request & req, Response & res) {
		SampleState & state = (SampleState &) app.getStateForIdentifier(StateIdentifier::sample);
		const size_t size = JSON_OBJECT_SIZE(4);
		StaticJsonDocument<size> doc;
		deserializeJson(doc, req.body);
		state.loadFrom(doc);
		res.end();
	});

	web.get("/clean", [](Request & req, Response & res) {
		res.contentType = "text/plain";
		res.send("success");
		res.end();
		app.transitionTo(StateIdentifier::clean);
		Serial.println("Transitioning to Cleaning State");
	});

	web.get("/preserve", [](Request & req, Response & res) {
		res.contentType = "text/plain";
		res.send("success");
		res.end();
		app.transitionTo(StateIdentifier::preserve);
		Serial.println("Transitioning to Preserving StateIdentifier");
	});

	web.get("/stop", [](Request & req, Response & res) {
		res.contentType = "text/plain";
		res.send("success");
		res.end();
		app.transitionTo(StateIdentifier::stop);
		Serial.println("Transitioning to Stop State");
	});

	web.get("/setTime", [](Request & req, Response & res) {
		StaticJsonDocument<JSON_OBJECT_SIZE(1)> doc;
		deserializeJson(doc, req.body);
		time_t currentTime = doc["utc-time"];
		int timeOffset = doc["local-time-offset"];
		power.printTime(currentTime);
		// power.rtc.set(currentTime); 	
		Serial.println("Setting RTC Time");	
		res.end();
	});

	web.get("/test", [](Request & req, Response & res) {
		Serial.println(req.body);
		res.end();
	});

	// <==> Serial Buffer <==>
	serialBuffer.didReceiveSerialCommand([](String & rawInput) {
		// Copy string into char array and convert it to uppercase
		int length = rawInput.length();
		char input[length + 1];
		input[length] = 0;
		rawInput.toCharArray(input, length);
		for (int i = 0; i < length; i++) {
			input[i] = toUpperCase(input[i]);
		}

		if (strstr(input, "SET TIME")) {
			unsigned long currentTime = power.compileTime() + millis() / 1000;
			power.rtc.set(currentTime);
			power.printTime(currentTime);
		} else if (strstr(input, "LOAD HOMEPAGE")) {
			web.homepage = card.loadContentsOfFile("index.txt");
		} else if (strstr(input, "LOG")) {
			card.logCurrentStatus();
		}
	});

	// addComponent(power);
	addComponent(web);
	// addComponent(shift);
	// addComponent(serialBuffer);
	addComponent(card);

	// <==> Pressure Sensor <==>
	// Note: Please refer to datasheet for values below
	// psEnable = true;
	if (psEnable) {
		Wire.begin();
		pressureSensor.setMinRaw(1638);      
		pressureSensor.setMaxRaw(14745);    
		pressureSensor.setMinPressure(0);   
		pressureSensor.setMaxPressure(30);  
		pressureSensor.start(); 
	}   

	// <==> Transition to Initial StateIdentifier <==>
	app.transitionTo(StateIdentifier::idle);
}

void ready() {
	setTimeout(1000, []() {
		// web.homepage = card.loadContentsOfFile("index.hml");
		web.printWiFiStatus();
		web.begin();
	});

	// Logging
	TimeElements tm;
	time_t currentTime = power.compileTime();
	breakTime(currentTime, tm);

	Status & status = Status::current();
	status.directory = new char[16]();
	status.filename = new char[16]();
	status.filepath = new char[32]();

	sprintf(status.directory, "logs/%u-%u-%u/", tm.Month, tm.Day, tmYearToY2k(tm.Year));
	sprintf(status.filename, "%d%u%u%u.csv", Status::current().currentValve, tm.Hour, tm.Minute, tm.Second);
	sprintf(status.filepath, "%s%s", status.directory, status.filename);
	card.makeDirectory(status.directory);

	Serial.println(status.filepath);
	setTime(power.compileTime()); // Remove this line when connected to RTC

	// Log every T interval
	// OPTask loggingTask("logging");
	// loggingTask.wait(1000, []() {
	// 	card.logCurrentStatus();
	// });
	// runForever(loggingTask);

	OPTask memory("memory");
	memory.wait(1000, []() {
		printFreeRam();
	}); 
	runForever(memory);
}

void update() {
	if (psEnable) {
		pressureSensor.update();
	}

	if (alarmTriggered && app.getCurrentState()->state->id == StateIdentifier::idle) {
		alarmTriggered = false;
		Serial.println("Alarm Triggered");
		app.transitionTo(StateIdentifier::flush);
	}
}

OPSystem app(prepare, ready, update);
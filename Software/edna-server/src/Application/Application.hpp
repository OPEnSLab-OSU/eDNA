#pragma once

#include <KPController.hpp>
#include <KPFileLoader.hpp>
#include <KPSerialInput.hpp>
#include <KPServer.hpp>
#include <KPStateMachine.hpp>
#include <KPAction.hpp>

#include <Application/Config.hpp>
#include <Application/Constants.hpp>
#include <Application/States.hpp>
#include <Application/Status.hpp>

#include <Components/Pump.hpp>
#include <Components/ShiftRegister.hpp>
#include <Components/Power.hpp>

#include <Valve/Valve.hpp>
#include <Valve/ValveManager.hpp>

#include <Utilities/JsonEncodableDecodable.hpp>

#define AirValveBitIndex	 2
#define AlcoholValveBitIndex 3
#define FlushValveBitIndex	 5

extern void printDirectory(File dir, int numTabs);

class Application : public KPController, KPSerialInputListener {
private:
	void setupServer() {
		// web.serveStaticFile("/gzip", "/index.gz", card, "text/html");
		// web.serveStaticFile("/", "/index.htm", fileLoader, "text/html");

		web.get("/", [this](Request & req, Response & res) {
			res.sendFile("combined.htm", fileLoader);
			res.end();
		});

		web.get("/gzip", [this](Request & req, Response & res) {
			res.setHeader("Content-Encoding", "gzip");
			res.sendFile("index.gz", fileLoader);
			res.end();
		});

		web.get("/api/status", [this](Request & req, Response & res) {
			const int size = ProgramSettings::STATUS_JSON_BUFFER_SIZE;

			StaticJsonDocument<size> doc;
			JsonObject object = doc.to<JsonObject>();
			status.encodeJSON(object);

			KPStringBuilder<10> length(measureJson(doc));

			res.setHeader("Content-Type", "application/json");
			res.setHeader("Content-Length", length.c_str());
			res.sendJSON(doc);
			res.end();
		});

		web.get("/api/valverefs", [this](Request & req, Response & res) {
			const size_t size = ProgramSettings::VALVEREF_JSON_BUFFER_SIZE * ProgramSettings::MAX_VALVES;
			StaticJsonDocument<size> doc;
			JsonArray valverefs = doc.to<JsonArray>();
			for (const Valve & v : vm.valves) {
				JsonObject obj = valverefs.createNestedObject();
				Valveref ref(v);
				ref.encodeJSON(obj);
				serializeJsonPretty(obj, Serial);
			}

			res.setHeader("Content-Type", "application/json");
			res.sendJSON(doc);
			res.end();
		});

		web.get("/stop", [this](Request & req, Response & res) {

		});

		web.post("/submit", [this](Request & req, Response & res) {

		});
	}

	void commandReceived(const String & line) override {
		if (line == "load status") status.load(config.statusFile);
		if (line == "save status") status.save(config.statusFile);
		if (line == "print status") println(status);

		if (line == "load config") config.load();
		if (line == "print config") println(config);

		if (line == "print sd") printDirectory(SD.open("/"), 0);

		if (line == "load valves") vm.loadValvesFromDirectory(config.valveFolder);
		if (line == "save valves") vm.saveValvesToDirectory(config.valveFolder);
		if (line == "print valves") {
			for (size_t i = 0; i < vm.valves.size(); i++) {
				println(vm.valves[i]);
			}
		}

		if (line == "reset valves") {
			for (int i = 0; i < config.valveUpperBound; i++) {
				vm.init(config);
			}
		}

		if (line == "read test") {
			char buffer[64];
			while (fileLoader.loadContentOfFile("index.gz", buffer, 64)) {
				println(buffer);
			}
		}

		if (line == "test") {
			KPArray<int, ProgramSettings::MAX_VALVES> task1 = vm.filter([](const Valve & v) {
				return strcmp(v.group, "Task 1") == 0;
			});

			for (size_t i = 0; i < task1.size(); i++) {
				println(task1[i], ",");
			}
		}
	}

public:
	KPServer web{"web-server", "eDNA-test", "password"};
	KPStateMachine sm{"state-machine"};
	KPFileLoader fileLoader{"file-loader", SDCard_Pin};
	KPActionScheduler<10> scheduler{"scheduler"};

	Pump pump{"pump", Motor_Forward_Pin, Motor_Reverse_Pin};
	Power power{"power"};
	ShiftRegister shift{"shift-register", 32, SR_Data_Pin, SR_Clock_Pin, SR_Latch_Pin};

	Config config{ProgramSettings::CONFIG_FILE_PATH};
	Status status;

	ValveManager vm;

private:
	void setup() override {
		KPController::setup();

		// Setup serial communication
		Serial.begin(115200);
		while (!Serial) {
		};
		// delay(5000);

		addComponent(sm);
		addComponent(web);
		addComponent(fileLoader);
		addComponent(shift);
		addComponent(pump);
		addComponent(scheduler);

		sm.registerState(StateIdle(), StateName::IDLE);
		sm.registerState(StateStop(), StateName::STOP);
		sm.registerState(StateFlush(), StateName::FLUSH);
		sm.registerState(StateSample(), StateName::SAMPLE);
		sm.registerState(StatePreserve(), StateName::PRESERVE);
		sm.registerState(StateDecon(), StateName::DECON);

		KPSerialInput::instance().addListener(this);

		config.load();
		status.init(config);

		vm.init(config);
		vm.addListener(&status);

		// Setup web server
		setupServer();
		web.begin();

		vm.loadValvesFromDirectory(config.valveFolder);
		// vm.saveValvesToDirectory(config().valveFolder);

		// StaticJsonDocument<200> doc;
		// JsonArray array = doc.to<JsonArray>();
		// {
		// 	JsonObject obj = array.createNestedObject();
		// 	char text[]	   = "Panda Cubs";
		// 	obj["text"].set((char *)text);
		// }
		// serializeJsonPretty(array, Serial);

		// Transition to idle state
		sm.transitionTo(StateName::IDLE);
	}

	void update() override {
		KPController::update();
	}
};
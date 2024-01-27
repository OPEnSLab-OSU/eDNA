#include <Application/App.hpp>
#include <Components/Sensors/FlowSensor.hpp>

namespace {
    struct SerialRequest {
        const char * cmd;
        const JsonArray & path;
        const JsonDocument & input;
    };

    using FunctionType = std::function<void(SerialRequest req)>;
    using StringToFunc = std::unordered_map<std::string, FunctionType>;
    StringToFunc mapNameToCallback;

    constexpr const char EOT = '\4';

    void endTransmission() {
        print(EOT);
    }
}  // namespace

// Registering Top-level serial commands
void App::setupSerialRouting() {
    mapNameToCallback["hyperflush"] = [this](SerialRequest req) {
        const auto response = dispatchAPI<API::StartHyperFlush>();
        serializeJson(response, Serial);
    };

    mapNameToCallback["debubble"] = [this](SerialRequest req) {
        const auto response = dispatchAPI<API::StartDebubble>();
        serializeJson(response, Serial);
    };

    mapNameToCallback["query"] = [this](SerialRequest req) {
        const char * endpoint = req.path[1];
        if (strcmp(endpoint, "status") == 0) {
            const auto & response = dispatchAPI<API::StatusGet>();
            serializeJson(response, Serial);
            endTransmission();
            return;
        }

        if (strcmp(endpoint, "config") == 0) {
            const auto & response = dispatchAPI<API::ConfigGet>();
            serializeJson(response, Serial);
            endTransmission();
            return;
        }

        if (strcmp(endpoint, "ram") == 0) {
            printFreeRam();
            endTransmission();
            return;
        }
    };

    mapNameToCallback["reset"] = [this](SerialRequest req) {
        const char * endpoint = req.path[1];
        if (strcmp(endpoint, "valves") == 0) {
            for (int i = 0; i < config.numberOfValves; i++) {
                vm.setValveStatus(i, ValveStatus::Code(config.valves[i]));
            }

            vm.writeToDirectory();

            StaticJsonDocument<ValveManager::encodingSize()> response;
            encodeJSON(vm, response.to<JsonVariant>());
            serializeJson(response, Serial);
            endTransmission();
            return;
        }
    };
}

/**
 *
 * {
 * 	path: ["top-level command", "sub1", "sub2"],
 * cmd: ""
 * 	...
 * }
 */

void App::commandReceived(const char * msg, size_t size) {
    if (msg[0] == '{') {
        StaticJsonDocument<512> input;
        deserializeJson(input, msg);

        for (auto & item : mapNameToCallback) {
            if (item.first == input["cmd"].as<const char *>()) {
                item.second({input["cmd"], input["path"], input});
                break;
            }
        }
    }

    // if (strcmp(msg, "flow temp")) {
    // 	// auto response = FlowSensor::read();
    // }

    // if (strcmp(msg, "flow flow")) {}
    // KPString line{msg};

    // println(line);

    // if (msg[0] == '{') {
    // 	StaticJsonDocument<255> doc;
    // 	deserializeJson(doc, msg);

    // 	StaticJsonDocument<255> response;
    // 	// dispatch(doc["cmd"].as<const char *>(), doc, response);
    // }

    if (strcmp(msg, "status")) {
        println(status);
    }

    // if (line == "print config") {
    // 	println(config);
    // }

    // if (line == "schedule now") {
    // 	println("Schduling temp task");
    // 	Task task				= tm.createTask();
    // 	task.schedule			= now() + 5;
    // 	task.flushTime			= 5;
    // 	task.sampleTime			= 5;
    // 	task.deleteOnCompletion = true;
    // 	task.status				= TaskStatus::active;
    // 	task.valves.push_back(0);
    // 	tm.insertTask(task);
    // 	println(scheduleNextActiveTask().description());
    // }

    if (strcmp(msg, "reset valves") == 0) {
        for (int i = 0; i < config.numberOfValves; i++) {
            vm.setValveStatus(i, ValveStatus::Code(config.valves[i]));
        }
    }

    // 	vm.writeToDirectory();
    // }

    // if (line == "mem") {
    // 	println(free_ram());
    // }

    // if (line == "hyperflush") {
    // 	beginHyperFlush();
    // };
}
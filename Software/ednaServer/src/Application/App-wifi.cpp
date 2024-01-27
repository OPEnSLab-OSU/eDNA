#include <Application/App.hpp>

void App::setupServerRouting() {
    server.handlers.reserve(13);

    server.get("/", [this](Request & req, Response & res) {
        if (strstr(req.header, "br")) {
            res.setHeader("Content-Encoding", "br");
            res.sendFile("index.br", fileLoader);
        } else {
            res.setHeader("Content-Encoding", "gzip");
            res.sendFile("index.gz", fileLoader);
        }
        res.end();
    });

    server.get("/api/preload", [this](Request &, Response & res) {
        const auto & response = dispatchAPI<API::StartHyperFlush>();
        res.json(response);
        res.end();
    });

    server.get("/api/alcohol-debubbler", [this](Request &, Response & res) {
        const auto & response = dispatchAPI<API::StartDebubble>();
        res.json(response);
        res.end();
    });

    // ────────────────────────────────────────────────────────────────────────────────
    // Get the current status
    // ────────────────────────────────────────────────────────────────────────────────
    server.get("/api/status", [this](Request &, Response & res) {
        const auto & response = dispatchAPI<API::StatusGet>();

        KPStringBuilder<10> length(measureJson(response));
        res.setHeader("Content-Length", length);
        res.json(response);
        res.end();

        serializeJsonPretty(response, Serial);
    });

    // ────────────────────────────────────────────────────────────────────────────────
    // Get a list of valve objects
    // ────────────────────────────────────────────────────────────────────────────────
    server.get("/api/valves", [this](Request &, Response & res) {
        StaticJsonDocument<ValveManager::encodingSize()> response;
        encodeJSON(vm, response.to<JsonArray>());

        KPStringBuilder<10> length(measureJson(response));
        res.setHeader("Content-Length", length);
        res.json(response);
        res.end();
    });

    // ────────────────────────────────────────────────────────────────────────────────
    // Get a list of task objects
    // ────────────────────────────────────────────────────────────────────────────────
    server.get("/api/tasks", [this](Request &, Response & res) {
        StaticJsonDocument<TaskManager::encodingSize()> response;
        encodeJSON(tm, response.to<JsonArray>());

        KPStringBuilder<10> length(measureJson(response));
        res.setHeader("Content-Length", length);
        res.json(response);
        res.end();
    });

    // ────────────────────────────────────────────────────────────────────────────────
    // Get now task object
    // ────────────────────────────────────────────────────────────────────────────────
    server.get("/api/nowtask", [this](Request &, Response & res) {
        println(BLUE("REQUESTING NOW TASK"));
        StaticJsonDocument<TaskManager::encodingSize()> response;
        encodeJSON(ntm, response.to<JsonArray>());

        KPStringBuilder<10> length(measureJson(response));
        res.setHeader("Content-Length", length);
        res.json(response);
        res.end();
    });

    // ────────────────────────────────────────────────────────────────────────────────
    // Get task with name
    // ────────────────────────────────────────────────────────────────────────────────
    server.post("/api/task/get", [this](Request & req, Response & res) {
        StaticJsonDocument<Task::encodingSize()> body;
        deserializeJson(body, req.body);
        serializeJsonPretty(body, Serial);

        const auto & response = dispatchAPI<API::TaskGet>(body);
        res.json(response);
        res.end();
    });

    // ────────────────────────────────────────────────────────────────────────────────
    // Create a new task with name
    // ────────────────────────────────────────────────────────────────────────────────
    server.post("/api/task/create", [this](Request & req, Response & res) {
        StaticJsonDocument<100> body;
        deserializeJson(body, req.body);
        serializeJsonPretty(body, Serial);

        const auto & response = dispatchAPI<API::TaskCreate>(body);
        res.json(response);
        serializeJsonPretty(response, Serial);
        res.end();
    });

    // ────────────────────────────────────────────────────────────────────────────────
    // Update existing task with incoming data
    // ────────────────────────────────────────────────────────────────────────────────
    server.post("/api/task/save", [this](Request & req, Response & res) {
        StaticJsonDocument<Task::encodingSize()> body;
        deserializeJson(body, req.body);
        serializeJsonPretty(body, Serial);

        const auto & response = dispatchAPI<API::TaskSave>(body);
        res.json(response);
        res.end();
    });

    // ────────────────────────────────────────────────────────────────────────────────
    // Update existing task with incoming data
    // ────────────────────────────────────────────────────────────────────────────────
    server.post("/api/nowtask/save", [this](Request & req, Response & res) {
        println(BLUE("SAVING NOW TASK"));
        StaticJsonDocument<Task::encodingSize()> body;
        deserializeJson(body, req.body);
        serializeJsonPretty(body, Serial);

        const auto & response = dispatchAPI<API::NowTaskSave>(body);
        res.json(response);
        res.end();
    });
    // ────────────────────────────────────────────────────────────────────────────────
    // Schedule a task (marking it active)
    // ────────────────────────────────────────────────────────────────────────────────
    server.post("/api/task/schedule", [this](Request & req, Response & res) {
        StaticJsonDocument<100> body;
        deserializeJson(body, req.body);
        serializeJsonPretty(body, Serial);

        const auto & response = dispatchAPI<API::TaskSchedule>(body);
        res.json(response);
        res.end();
    });

    // ────────────────────────────────────────────────────────────────────────────────
    // Unschedule a task (making it inactive)
    // ────────────────────────────────────────────────────────────────────────────────
    server.post("/api/task/unschedule", [this](Request & req, Response & res) {
        StaticJsonDocument<100> body;
        deserializeJson(body, req.body);
        serializeJsonPretty(body, Serial);

        const auto & response = dispatchAPI<API::TaskUnschedule>(body);
        res.json(response);
        res.end();
    });

    // ────────────────────────────────────────────────────────────────────────────────
    // Delete task with name
    // ────────────────────────────────────────────────────────────────────────────────
    server.post("/api/task/delete", [this](Request & req, Response & res) {
        StaticJsonDocument<100> body;
        deserializeJson(body, req.body);

        const auto & response = dispatchAPI<API::TaskDelete>(body);
        res.json(response);
        res.end();
    });

    // ────────────────────────────────────────────────────────────────────────────────
    // RTC update
    // ────────────────────────────────────────────────────────────────────────────────
    server.post("/api/rtc/update", [this](Request & req, Response & res) {
        StaticJsonDocument<100> body;
        deserializeJson(body, req.body);

        const auto & response = dispatchAPI<API::RTCUpdate>(body);
        res.json(response);
        res.end();
    });

    // ────────────────────────────────────────────────────────────────────────────────
    // Pressure Cutoff update
    // ────────────────────────────────────────────────────────────────────────────────
    server.post("/api/pressure/update", [this](Request & req, Response & res) {
        StaticJsonDocument<100> body;
        deserializeJson(body, req.body);

        const auto & response = dispatchAPI<API::PressureUpdate>(body);
        res.json(response);
        res.end();
    });

    // ────────────────────────────────────────────────────────────────────────────────
    // Emergency stop
    // ────────────────────────────────────────────────────────────────────────────────
    server.get("/stop", [this](Request & req, Response & res) { newStateController.stop(); });

    server.get("/api/valves/reset", [this](Request & req, Response & res) {
        for (int i = 0; i < config.numberOfValves; i++) {
            vm.setValveStatus(i, ValveStatus::Code(config.valves[i]));
        }
        vm.writeToDirectory();
        ntm.task.valve = 0;
        res.end();
    });
}
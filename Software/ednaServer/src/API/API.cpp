#include <Application/App.hpp>

namespace API {
    auto StartHyperFlush::operator()(App & app) -> R {
        decltype(auto) hyperFlushName = app.hyperFlushStateController.getCurrentState()->getName();

        R response;
        if (strcmp(HyperFlush::IDLE, hyperFlushName) == 0) {
            app.beginHyperFlush();
            response["success"] = "Begin preloading water";
        } else {
            response["error"] = "Preloading water is already in operation";
        }

        return response;
    }

    auto StartNowTask::operator()(App & app) -> R {
        decltype(auto) nowTaskName = app.nowTaskStateController.getCurrentState()->getName();

        R response;
        if (strcmp(NowT::IDLE, nowTaskName) == 0) {
            app.beginNowTask();
            response["success"] = "Beginning Now Task";
        } else {
            response["error"] = "Already Sampling";
        }
        
        return response;
    }

    auto StartDebubble::operator()(App & app) -> R {
        decltype(auto) debubbleName = app.debubbleStateController.getCurrentState()->getName();

        R response;
        if (strcmp(Debubble::IDLE, debubbleName) == 0) {
            app.beginDebubble();
            response["success"] = "Begin preloading water";
        } else {
            response["error"] = "Preloading water is already in operation";
        }

        return response;
    }

    auto StatusGet::operator()(App & app) -> R {
        R response;
        encodeJSON(app.status, response.to<JsonObject>());
        response["utc"] = now();
        return response;
    }

    auto ConfigGet::operator()(App & app) -> R {
        R response;
        encodeJSON(app.config, response.to<JsonObject>());
        return response;
    }

    auto TaskCreate::operator()(App & app, JsonDocument & input) -> R {
        R response;
        const char * name = input[TaskKeys::NAME];

        // Create and add new task to manager
        Task task = app.tm.createTask();
        strcpy(task.name, name);
        app.tm.insertTask(task, true);

        // NOTE: Uncomment to save task. Not sure if this is necessary here.
        // Current behaviour requires the user to "save" the task first before writing to SD card.
        // tm.writeToDirectory();

        // Success response
        KPStringBuilder<100> success("Successfully created ", name);
        response["success"] = (char *) success;

        // Return task with partially filled fields
        JsonVariant payload = response.createNestedObject("payload");
        encodeJSON(task, payload);

        println(measureJson(response));
        serializeJsonPretty(response, Serial);
        return response;
    }

    auto TaskGet::operator()(Arg<0> & app, Arg<1> & input) -> R {
        R response;

        int id = input[TaskKeys::ID];
        if (app.tm.findTask(id)) {
            JsonVariant payload = response.createNestedObject("payload");
            encodeJSON(app.tm.tasks[id], payload);
            response["success"] = "Task found";
        } else {
            response["error"] = "Task not found";
        }

        return response;
    }

    auto TaskSave::operator()(Arg<0> & app, Arg<1> & input) -> R {
        R response;
        // Prarse incomming payload
        Task incomingTask;
        incomingTask.decodeJSON(input.as<JsonVariant>());

        // Validate
        app.validateTaskForSaving(incomingTask, response);
        if (response.containsKey("error")) {
            return response;
        }

        // Save
        app.tm.tasks[incomingTask.id] = incomingTask;
        app.tm.writeToDirectory();

        response["success"] = "Task successfully saved";
        return response;
    }

    auto NowTaskSave::operator()(Arg<0> & app, Arg<1> & input) -> R {
        R response;
        // Prarse incomming payload
        NowTask incomingTask;
        incomingTask.decodeJSON(input.as<JsonVariant>());


        // Save
        app.ntm.task = incomingTask;
        app.ntm.writeToDirectory();

        response["success"] = "Task successfully saved";
        return response;
    }

    auto TaskDelete::operator()(Arg<0> & app, Arg<1> & input) -> R {
        R response;
        int id = input["id"];

        if (!app.tm.findTask(id)) {
            response["error"] = "No task with this name";
            return response;
        }

        if (app.tm.tasks[id].status == TaskStatus::active) {
            response["error"] = "Task currently have active status. "
                                "Please deactivate before continue.";
            return response;
        }

        app.tm.deleteTask(id);
        app.tm.writeToDirectory();
        response["success"] = "Task deleted";
        return response;
    }

    auto TaskSchedule::operator()(Arg<0> & app, Arg<1> & input) -> R {
        R response;
        int id = input[TaskKeys::ID];

        app.validateTaskForScheduling(id, response);
        if (response.containsKey("error")) {
            return response;
        }

        Task & task           = app.tm.tasks[id];
        task.valveOffsetStart = 0;
        app.tm.setTaskStatus(task.id, TaskStatus::active);
        app.tm.writeToDirectory();

        JsonVariant payload = response.createNestedObject("payload");
        encodeJSON(task, payload);

        ScheduleReturnCode code = app.scheduleNextActiveTask();
        println(code.description());

        response["success"] = "Task has been scheduled";
        return response;
    }

    auto TaskUnschedule::operator()(Arg<0> & app, Arg<1> & input) -> R {
        R response;
        int id = input[TaskKeys::ID];

        if (!app.tm.findTask(id)) {
            response["error"] = "Task not found";
            return response;
        }

        Task & task = app.tm.tasks[id];
        if (app.currentTaskId == task.id) {
            app.invalidateTaskAndFreeUpValves(task);
            app.newStateController.stop();
        } else {
            app.invalidateTaskAndFreeUpValves(task);
            app.tm.writeToDirectory();
        }

        JsonVariant payload = response.createNestedObject("payload");
        encodeJSON(task, payload);
        response["success"] = "Task is now inactive";
        return response;
    }

    auto RTCUpdate::operator()(Arg<0> & app, Arg<1> & input) -> R {
        R response;
        const time_t utc         = input["utc"];
        const int timezoneOffset = input["timezoneOffset"];
        const auto compileTime   = app.power.compileTime(timezoneOffset);

#ifdef DEBUG
        println("Received UTC: ", utc);
        println("Current RTC time: ", now());
        println("Time at compilation: ", compileTime);
#endif

        // Checking against compiled time + millis() to prevents bogus value
        if (utc >= compileTime + millisToSecs(millis())) {
            response["success"] = "RTC updated";
            app.power.set(utc + 1);
        } else {
            response["error"] = "That doesn't seem right.";
        }

        return response;
    }

    auto PressureUpdate::operator()(Arg<0> & app, Arg<1> & input) -> R {
        R response;
        const float NewPressureCutOff = input["pressure"];

        // Prevent bogus value
        if (NewPressureCutOff >= 0.0) {
            response["success"] = "Pressure Cutoff updated";
            app.status.cutoffPressure = NewPressureCutOff;
        } else {
            response["error"] = "That doesn't seem right.";
        }

        return response;
    }
}  // namespace API
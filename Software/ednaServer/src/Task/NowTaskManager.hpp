#pragma once
#include <KPFoundation.hpp>
#include <KPSubject.hpp>
#include <KPDataStoreInterface.hpp>

#include <Task/NowTask.hpp>
#include <Task/NowTaskObserver.hpp>
#include <Task/TaskObserver.hpp>
#include <Application/Config.hpp>

#include <vector>
#include "SD.h"

class NowTaskManager : public KPComponent,
                    public JsonEncodable,
                    public Printable,
                    public KPSubject<NowTaskObserver> {

public:
    const char * taskFolder = nullptr;
    NowTask task;

    NowTaskManager() : KPComponent("NowTaskManager") {}

    void init(Config & config) {
        taskFolder = config.taskFolder;
    }

    bool advanceTask() {
        task.valve++;
        return markTaskAsCompleted();
}

    bool setTaskStatus(TaskStatus status) {
        task.status = status;
        updateObservers(&NowTaskObserver::nowTaskDidUpdate, task);
        return true;
    }

    int numberOfActiveTasks() const {
        if(task.status == TaskStatus::active){
            return 1;
        }
        return 0;
    }

    
    bool markTaskAsCompleted() {

        task.status = TaskStatus::completed;
        updateObservers(&NowTaskObserver::nowTaskDidComplete, task);

        return task.isCompleted();
    }

    /** ────────────────────────────────────────────────────────────────────────────
     *  @brief Load all tasks object from the specified directory in the SD card
     *
     *  @param _dir Path to tasks directory (default=~/tasks)
     *  ──────────────────────────────────────────────────────────────────────────── */
    void loadTasksFromDirectory(const char * _dir = nullptr) {
        const char * dir = _dir ? _dir : taskFolder;

        JsonFileLoader loader;
        loader.createDirectoryIfNeeded(dir);

        auto start = millis();
        KPStringBuilder<32> filepath(dir, "/nowtask.js");
        loader.load(filepath, task);

        println(GREEN("Now Task Manager"), " finished reading in ", millis() - start, " ms\n");
        // updateObservers(&TaskObserver::taskCollectionDidUpdate, tasks.begin());
    }


    /** ────────────────────────────────────────────────────────────────────────────
     *  @brief Write task array to SD directory
     *
     *  @param _dir Path to tasks directory (default=~/tasks)
     *  ──────────────────────────────────────────────────────────────────────────── */
    void writeToDirectory(const char * _dir = nullptr) {
        const char * dir = _dir ? _dir : taskFolder;

        JsonFileLoader loader;
        loader.createDirectoryIfNeeded(dir);

        println("Writing Now Task");

        KPStringBuilder<32> filename("nowtask.js");
        KPStringBuilder<64> filepath(dir, "/", filename);
        loader.save(filepath, task);
    }

#pragma region JSONENCODABLE
    static const char * encoderName() {
        return "NowTaskManager";
    }

    static constexpr size_t encodingSize() {
        return NowTask::encodingSize() * 5;
    }

    bool encodeJSON(const JsonVariant & dst) const override {
        JsonVariant obj = dst.createNestedObject();
        if (!task.encodeJSON(obj)) {
            return false;
        }
        return true;
    }
#pragma endregion
#pragma region PRINTABLE
    size_t printTo(Print & p) const {
        size_t charWritten = 0;
        charWritten += p.println("[");
        charWritten += p.print(task);
        /*for (const auto & kv : tasks) {
            charWritten += p.print(kv.second);
            charWritten += p.println(",");
        }*/

        return charWritten + p.println("]");
    }
#pragma endregion
};

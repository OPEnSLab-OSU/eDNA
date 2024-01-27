#pragma once
#include <KPFoundation.hpp>
#include <KPSubject.hpp>
#include <KPDataStoreInterface.hpp>

#include <Task/Task.hpp>
#include <Task/TaskObserver.hpp>
#include <Application/Config.hpp>

#include <vector>
#include "SD.h"

class TaskManager : public KPComponent,
                    public JsonEncodable,
                    public Printable,
                    public KPSubject<TaskObserver> {
public:
    using CollectionType = std::unordered_map<int, Task>;
    using EntryType      = CollectionType::value_type;
    CollectionType tasks;

public:
    const char * taskFolder = nullptr;

    TaskManager() : KPComponent("TaskManager") {}

    void init(Config & config) {
        taskFolder = config.taskFolder;
    }

    int generateTaskId() const {
        return random(1, RAND_MAX);
    }

    Task createTask() {
        const auto timenow = now();
        Task task;
        task.id        = generateTaskId();
        task.createdAt = timenow;
        task.schedule  = timenow;
        return task;
    }

    const CollectionType & taskCollection() const {
        return tasks;
    }

    bool advanceTask(int id) {
        if (!findTask(id)) {
            return false;
        }

        auto & task = tasks[id];
        println(GREEN("Task Time betwen: "), task.timeBetween);
        task.schedule = now() + std::max(task.timeBetween, 5);
        if (++task.valveOffsetStart >= task.getNumberOfValves()) {
            return markTaskAsCompleted(id);
        }

        return true;
    }

    bool setTaskStatus(int id, TaskStatus status) {
        if (tasks.find(id) == tasks.end()) {
            return false;
        }

        tasks[id].status = status;
        updateObservers(&TaskObserver::taskDidUpdate, tasks[id]);
        return true;
    }

    int numberOfActiveTasks() const {
        return std::count_if(tasks.begin(), tasks.end(), [](const EntryType & kv) {
            return kv.second.status == TaskStatus::active;
        });
    }

    bool markTaskAsCompleted(int id) {
        if (tasks.find(id) == tasks.end()) {
            return false;
        }

        updateObservers(&TaskObserver::taskDidComplete);
        auto & task = tasks[id];
        task.valves.clear();
        if (task.deleteOnCompletion) {
            println("DELETED: ", id);
            deleteTask(id);
        } else {
            task.status = TaskStatus::completed;
            updateObservers(&TaskObserver::taskDidUpdate, task);
        }

        return true;
    }

    bool findTask(int id) const {
        return tasks.find(id) != tasks.end();
    }

    bool deleteTask(int id) {
        if (tasks.erase(id)) {
            updateObservers(&TaskObserver::taskDidDelete, id);
            return true;
        }

        return false;
    }

    int deleteIf(std::function<bool(const Task &)> predicate) {
        int oldSize = tasks.size();
        for (auto it = tasks.begin(); it != tasks.end();) {
            if (predicate(it->second)) {
                auto id = it->first;
                it      = tasks.erase(it);
                updateObservers(&TaskObserver::taskDidDelete, id);
            } else {
                it++;
            }
        }

        return oldSize - tasks.size();
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

        // Load task index file and get the number of tasks
        KPStringBuilder<32> indexFilepath(dir, "/index.js");
        StaticJsonDocument<100> indexFile;
        loader.load(indexFilepath, indexFile);

        // Decode each task object into memory
        int count  = indexFile["count"];
        auto start = millis();
        for (int i = 0; i < count; i++) {
            KPStringBuilder<32> filepath(dir, "/task-", i, ".js");
            Task task;
            loader.load(filepath, task);
            tasks.insert({task.id, task});
        }

        println(GREEN("Task Manager"), " finished reading in ", millis() - start, " ms\n");
        // updateObservers(&TaskObserver::taskCollectionDidUpdate, tasks.begin());
    }

    /** ────────────────────────────────────────────────────────────────────────────
     *  @brief Get the Active Task Ids sorted by their schedules (<)
     *
     *  @return std::vector<int> list of ids
     *  ──────────────────────────────────────────────────────────────────────────── */
    std::vector<int> getActiveSortedTaskIds() {
        std::vector<int> result;
        result.reserve(tasks.size());

        for (const auto & kv : tasks) {
            if (kv.second.status == TaskStatus::active) {
                result.push_back(kv.first);
            }
        }

        std::sort(result.begin(), result.end(),
                  [this](int a, int b) { return tasks[a].schedule < tasks[b].schedule; });

        return result;
    }

    /** ────────────────────────────────────────────────────────────────────────────
     *  @brief Insert task into TaskManager's internal data structure
     *
     *  @param task Task object to be inserted
     *  @param forcedIdGeneration Forced ID generation if task.id already exists
     *  @return bool true on successful insertion, false otherwise
     *  ──────────────────────────────────────────────────────────────────────────── */
    bool insertTask(Task & task, bool forcedIdGeneration = false) {
        if (forcedIdGeneration) {
            while (!tasks.insert({task.id, task}).second) {
                task.id = random(RAND_MAX);
            }

            return true;
        }

        return tasks.insert({task.id, task}).second;
    }

    /** ────────────────────────────────────────────────────────────────────────────
     *  @brief Update the index file containing info about tasks
     *
     *  @param _dir Path to tasks directory (default=~/tasks)
     *  ──────────────────────────────────────────────────────────────────────────── */
    void updateIndexFile(const char * _dir = nullptr) {
        const char * dir = _dir ? _dir : taskFolder;

        JsonFileLoader loader;
        loader.createDirectoryIfNeeded(dir);

        KPStringBuilder<32> indexFilepath(dir, "/index.js");
        StaticJsonDocument<100> indexJson;
        indexJson["count"] = tasks.size();
        loader.save(indexFilepath, indexJson);
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

        println("Number of tasks to write: ", tasks.size());

        int i = 0;
        for (auto & kv : tasks) {
            KPStringBuilder<32> filename("task-", i, ".js");
            KPStringBuilder<64> filepath(dir, "/", filename);
            loader.save(filepath, kv.second);
            i++;
        }

        updateIndexFile(dir);
    }

#pragma region JSONENCODABLE
    static const char * encoderName() {
        return "TaskManager";
    }

    static constexpr size_t encodingSize() {
        return Task::encodingSize() * 5;
    }

    bool encodeJSON(const JsonVariant & dst) const override {
        for (const auto & kv : tasks) {
            JsonVariant obj = dst.createNestedObject();
            if (!kv.second.encodeJSON(obj)) {
                return false;
            }
        }

        return true;
    }
#pragma endregion
#pragma region PRINTABLE
    size_t printTo(Print & p) const {
        size_t charWritten = 0;
        charWritten += p.println("[");
        for (const auto & kv : tasks) {
            charWritten += p.print(kv.second);
            charWritten += p.println(",");
        }

        return charWritten + p.println("]");
    }
#pragma endregion
};

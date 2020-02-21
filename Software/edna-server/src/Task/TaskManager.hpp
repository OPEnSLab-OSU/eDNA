#pragma once
#include <KPFoundation.hpp>
#include <KPDataStoreInterface.hpp>
#include <KPArray.hpp>

#include <Task/Task.hpp>
#include <vector>

#include <Application/Config.hpp>

class TaskManager {
public:
	KPArray<Task, ProgramSettings::MAX_VALVES> tasks;

	TaskManager() {
		tasks.resize(ProgramSettings::MAX_VALVES);
	}

	bool createDirectoryIfNeeded(const char * dir) {
		File folder = SD.open(dir, FILE_READ);
		if (folder && !folder.isDirectory()) {
			raise(Error("TaskManager: path already exists but not directory"));
		}

		if (folder && folder.isDirectory()) {
			return true;
		}

		if (!folder) {
			bool success = SD.mkdir(dir);
			print("TaskManager: creating directory...");
			println(success ? "success" : "failed");
			return success;
		} else {
			return true;
		}
	}

	void loadTasksFromDirectory(const char * dir) {
		createDirectoryIfNeeded(dir);
		for (int i = 0; i < tasks.size(); i++) {
			KPStringBuilder<32> filename("task-", i < 10 ? "0" : "", i, ".js");
			KPStringBuilder<64> filepath(dir, "/", filename);
			tasks[i].load(filepath);
			tasks[i].valve_id = i;
			println(tasks[i]);
		}
	}

	void updateTasks(const JsonArrayConst & task_array) {
		for (const JsonObjectConst & object : task_array) {
			int id = object["valve"];
			if (tasks[id].active) {
				tasks[id].decodeJSON(object);
			} else {
				println("Task is already inactive");
			}
		}
	}

	void saveTasksToDirectory(const char * dir) {
		createDirectoryIfNeeded(dir);
		for (int i = 0; i < tasks.size(); i++) {
			KPStringBuilder<32> filename("task-", i < 10 ? "0" : "", i, ".js");
			KPStringBuilder<64> filepath(dir, "/", filename);
			tasks[i].save(filepath);
		}
	}
};
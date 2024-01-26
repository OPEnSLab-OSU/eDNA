#pragma once
#include "ArduinoJson.h"
#include "KPEventEmitter.hpp"
#include "KPSDCard.hpp"
#include "Time.h"

#define TaskDocumentSize 1000
#define TaskRefSize 300

// Note that time is UTC time since Jan 1st 1970 00:00:00
struct KPTaskRef {
	int id;
	char name[50];
};

struct KPTask {
    int id								{-1};
    long creation						{0};
	long schedule						{0};
	long interval						{0};
	long estimatedCompletion			{0};

	
	
	char name[50] 						{0}; 
	char currentState[32]				{0};
    char notes[250]						{0};

    int valves[KPStatus::valveCount]	{0};
	int numberOfValvesAssigned			{0};

	long flushTime						{0};
	long flushVolume					{0};

	long sampleTime						{0};
	long samplePressure					{0};
	long sampleVolume					{0};

	long cleanTime						{0};

	long preserveTime					{0};

	KPTask() {}
	KPTask(const JsonObject & task) {
		decode(task);
	}

    void decode(const JsonObject & task) {
		id = task[TaskKey::id];
		creation = task[TaskKey::creation];
        schedule = task[TaskKey::schedule];
        interval = task[TaskKey::interval];
		strcpy(currentState, task["currentState"]);

		JsonArray valves_json = task[TaskKey::valves];
        copyArray(valves_json, valves);
		numberOfValvesAssigned = valves_json.size();
    }

    void encode(JsonObject & task) const {
		task[TaskKey::id] = id;
		task[TaskKey::name] = name;
        task[TaskKey::schedule] = schedule;
        task[TaskKey::creation] = creation;
        task[TaskKey::interval] = interval;
		task["currentState"] = currentState;
        task[TaskKey::notes] = notes;
    }

	bool validate() const {
		return 
		id						>= 0			&&
		schedule 				>= now() 		&& 
		interval 				>= 0 			&& 
		numberOfValvesAssigned 	>= 1 			&&
		flushTime				>= 1			&&
		flushVolume				>= 1			&&
		sampleTime				>= 1			&&
		samplePressure			>= 1			&&
		sampleVolume			>= 1			&&
		cleanTime				>= 1			&&
		preserveTime			>= 1			;
	}

	operator bool() const {
		return validate();
	}

	void next() {
		schedule = now() + interval;
		numberOfValvesAssigned = max(0, numberOfValvesAssigned - 1);
	}

	void updateEstimatedCompletion() {
		long maximumRunningTimePerValve = flushTime + sampleTime + cleanTime + preserveTime;
		estimatedCompletion = now() + maximumRunningTimePerValve * numberOfValvesAssigned;
	}
};

class KPTaskManager : public KPEventEmitter {
public:
    KPTask currentTask;
    KPSDCard & card;

    ulong completionTimestamp			{0};
	bool isRunning						{0};
    bool pendingNextTask				{0};
    char sessionPath[32]				{0};

    KPTaskManager(KPSDCard & card) : card(card) {}

    void setup() {
        updateSessionPath();
    }

	void updateSessionPath() {
        card.pathToLastestSession(sessionPath);
    }

	void taskIdToFilepath(int id, char * buffer) {
        sprintf(buffer, "%s%s%0*d%s", sessionPath, "T", 4, id, ".js");
	}

	void saveTaskToStorage(KPTask & task) {

	} 

    void saveCurrentTaskToStorage(JsonDocument & doc) {
        JsonObject task = doc.as<JsonObject>();
        currentTask.encode(task);
        notify<JsonObject>(Event::TMTaskSaving, &task);

        // Construct path to the upcoming tasks file
        char filepath[64];
        taskIdToFilepath(task["id"].as<int>(), filepath);

		// Open the task file and serialize to the specified JSON document
        File taskFile = SD.open(filepath, FILE_WRITE);
        serializeJson(doc, taskFile);
		taskFile.close();
    }

	void updateTaskrefs(JsonArray & refs) {
		char filepath[64];
		sprintf(filepath, "%s%s", sessionPath, "taskrefs.js");

		File taskrefsFile = SD.open(filepath, FILE_WRITE);
		serializeJson(refs, taskrefsFile);
		taskrefsFile.close();
	}

	bool loadJsonFromStorage(char * name, JsonDocument & doc) {
		return loadJsonFromStorage((const char *) name, doc);
	}

	bool loadJsonFromStorage(const char * name, JsonDocument & doc) {
		char filepath[64];
        sprintf(filepath, "%s%s%s", sessionPath, name, ".js");
        Serial.println(filepath);
		return card.loadJsonFromFile(filepath, doc);
	}

    bool loadTaskRefsFromStorage(JsonDocument & doc) {
        return loadJsonFromStorage("taskrefs", doc);
    }

    bool loadTaskFromStorage(int id, JsonDocument & doc) {
        char filename[12];
        sprintf(filename, "%s%0*d", "T", 4, id);
        return loadJsonFromStorage(filename, doc);
    }

    bool loadNextTaskFromStorage() {
        // Load tasks into JSON document
        StaticJsonDocument<TaskRefSize * 24> activeDoc;
        bool success = loadJsonFromStorage("active", activeDoc);
		if (!success) {
            return false;
        }

        // Copy to local array
        JsonArray activeTaskRefs = activeDoc.as<JsonArray>();
        JsonObjectConst taskref_array[activeTaskRefs.size()];
        copyArray(activeTaskRefs, taskref_array, activeTaskRefs.size());

        // Get index of the task with the lowest schedule time
        int index = arrayPivot<JsonObjectConst>(taskref_array, activeTaskRefs.size(), [](JsonObjectConst & pivot, JsonObjectConst & next) {
            ulong schedule = pivot["schedule"];
			return schedule < next["schedule"] && schedule > now();
        });

		if (index == -1) {
			return false;
		}

        // Get the task from storage using taskRef id and check the validity of the task
        JsonObject activeTaskRef = activeTaskRefs[index];
		if (activeTaskRef["schedule"] < now()) {
			return false;
		}

        //  Construct a JSON document and load task ƒrom file into the current task object
        StaticJsonDocument<TaskDocumentSize> taskdoc;
        loadTaskFromStorage(activeTaskRef["id"], taskdoc);
        JsonObject next = taskdoc.as<JsonObject>();
        currentTask.decode(next);

        // Notify listeners
        notify<JsonObject>(Event::TMTaskLoaded, &next);
		return true;
	}

	// Search in storage the task with the specified if and replace it with the given task
	bool updateTaskFile(int id, const JsonObject & taskJson) {
		char filepath[64];
		taskIdToFilepath(id, filepath);

		if (!SD.exists(filepath)) {
			return false;
		}

		// Delete the file and create a new file 
		SD.remove(filepath);
		File taskFile = SD.open(filepath, FILE_WRITE);
		serializeJson(taskJson, taskFile);
		taskFile.close();
		return true;
	}

	bool updateTaskFile(const KPTask & task) {
		if (task.id < 0) {
			return false;
		}

		StaticJsonDocument<TaskDocumentSize> doc;
		JsonObject taskJson = doc.as<JsonObject>();
		task.encode(taskJson);
		return updateTaskFile(task.id, taskJson);
	}

	void markCurrentTaskAsComplete() {
		currentTask.next();
		updateTaskFile(currentTask);
		if (currentTask.numberOfValvesAssigned == 0) {
			markTaskUnactive(currentTask.id);
		}

		// currentTask.markAsComplete();
	}

	bool markTaskUnactive(int id) {
		StaticJsonDocument<TaskRefSize * 10> taskrefs;
		loadTaskRefsFromStorage(taskrefs);

		JsonArray taskrefs_array = taskrefs.as<JsonArray>();
		for (int i = 0; i < taskrefs_array.size(); i++) {
			JsonObject task = taskrefs_array[i];
			if (id == task["id"]) {
				// Free up the valves
				while (task["valves"].size() > 0) {
					task["valves"].remove(0);
				} 

				task["status"] = "Unactive";
				updateTaskrefs(taskrefs_array);
				Serial.println("Update taskrefs file");
				return true;
			}
		}

		return false;
	}

	void updateCurrentTaskState(const char * name) {
		strcpy(currentTask.currentState, name);
	}

    virtual void updateTaskManager() {}
};
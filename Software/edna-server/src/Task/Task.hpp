#pragma once
#include <KPArray.hpp>
#include <KPFoundation.hpp>
#include <array>

#include <ArduinoJson.h>

#include <Application/Constants.hpp>
#include <Utilities/JsonEncodableDecodable.hpp>
#include <Utilities/JsonFileLoader.hpp>

struct Taskref;
struct Task : public JsonEncodable, public JsonDecodable, public Printable {
	char name[TaskSettings::NAME_LENGTH]{0};
	char group[TaskSettings::GROUP_LENGTH]{0};
	char notes[TaskSettings::NOTES_LENGTH]{0};

	int valve_id  = -1;
	long creation = 0;
	long schedule = 0;
	bool active	  = false;

	unsigned long flushTime		 = 0;
	unsigned long flushVolume	 = 0;
	unsigned long sampleTime	 = 0;
	unsigned long samplePressure = 0;
	unsigned long sampleVolume	 = 0;
	unsigned long dryTime		 = 0;
	unsigned long preserveTime	 = 0;

	Task()					 = default;
	Task(const Task & other) = default;
	Task & operator=(const Task &) = default;

	explicit Task(const JsonObjectConst & data) {
		decodeJSON(data);
	}

	const char * decoderName() const override {
		return "Task";
	}

	const char * encoderName() const override {
		return "Task";
	}

	void load(const char * filepath) override {
		JsonFileLoader loader;
		loader.load<ProgramSettings::TASK_JSON_BUFFER_SIZE>(filepath, *this);
	}

	void save(const char * filepath) const override {
		JsonFileLoader loader;
		loader.save<ProgramSettings::TASK_JSON_BUFFER_SIZE>(filepath, *this);
	}

	void decodeJSON(const JsonObjectConst & source) override {
		using namespace TaskSettings;
		// -> name
		strncpy(name, source["name"], NAME_LENGTH);
		if (name[NAME_LENGTH - 1] != 0) {
			println("Warning (Task): Name exceeds its buffer size and will be truncated");
		}

		// -> group
		strncpy(group, source["group"], GROUP_LENGTH);
		if (group[GROUP_LENGTH - 1] != 0) {
			println("Warning (Task): Group name exceeds its buffer size and will be truncated");
		}

		// -> notes
		strncpy(notes, source["notes"], NOTES_LENGTH);
		if (notes[NOTES_LENGTH - 1] != 0) {
			println("Warning (Task): Notes exceeds its buffer size and will be truncated");
		}

		valve_id	   = source["valve"];
		creation	   = source["creation"];
		schedule	   = source["schedule"];
		active		   = source["schedule"];
		flushTime	   = source["flushTime"];
		flushVolume	   = source["flushVolume"];
		sampleTime	   = source["sampleTime"];
		samplePressure = source["samplePressure"];
		sampleVolume   = source["sampleVolume"];
		dryTime		   = source["dryTime"];
		preserveTime   = source["preserveTime"];
	}

	bool encodeJSON(JsonObject & dest) const override {
		return dest["name"].set(name)
			   && dest["group"].set(group)
			   && dest["notes"].set(notes)
			   && dest["valve"].set(valve_id)
			   && dest["creation"].set(creation)
			   && dest["schedule"].set(schedule)
			   && dest["schedule"].set(active)
			   && dest["flushTime"].set(flushTime)
			   && dest["flushVolume"].set(flushVolume)
			   && dest["sampleTime"].set(sampleTime)
			   && dest["samplePressure"].set(samplePressure)
			   && dest["sampleVolume"].set(sampleVolume)
			   && dest["dryTime"].set(dryTime)
			   && dest["preserveTime"].set(preserveTime);
	}

	size_t printTo(Print & printer) const override {
		StaticJsonDocument<ProgramSettings::TASK_JSON_BUFFER_SIZE> doc;
		JsonObject object = doc.to<JsonObject>();
		encodeJSON(object);
		return serializeJsonPretty(object, Serial);
	}
};

struct Taskref : public JsonEncodable, public JsonDecodable, public Printable {
	char name[TaskSettings::NAME_LENGTH]{0};
	char group[TaskSettings::GROUP_LENGTH]{0};
	int valve_id = -1;

	Taskref()					   = default;
	Taskref(const Taskref & other) = default;
	Taskref & operator=(const Taskref &) = default;

	Taskref(const Task & task) {
		strcpy(name, task.name);
		strcpy(group, task.group);
		valve_id = task.valve_id;
	}

	Taskref(const JsonObjectConst & data) {
		decodeJSON(data);
	}

	const char * decoderName() const {
		return "Taskref";
	}

	const char * encoderName() const {
		return "Taskref";
	}

	void load(const char * filepath) override {
		JsonFileLoader loader;
		loader.load<ProgramSettings::TASKREF_JSON_BUFFER_SIZE>(filepath, *this);
	}

	void save(const char * filepath) const override {
		JsonFileLoader loader;
		loader.save<ProgramSettings::TASKREF_JSON_BUFFER_SIZE>(filepath, *this);
	}

	void decodeJSON(const JsonObjectConst & source) override {
		using namespace TaskSettings;
		strncpy(name, source["name"], NAME_LENGTH);
		if (name[NAME_LENGTH - 1] != 0) {
			println("Warning (Task): Name exceeds its buffer size and will be truncated");
		}

		// -> group
		strncpy(group, source["group"], GROUP_LENGTH);
		if (group[GROUP_LENGTH - 1] != 0) {
			println("Warning (Task): Group name exceeds its buffer size and will be truncated");
		}

		// -> valve
		valve_id = source["valve"];
	}

	bool encodeJSON(JsonObject & dest) const override {
		return dest["name"].set(name)
			   && dest["group"].set(group)
			   && dest["valve"].set(valve_id);
	}

	size_t printTo(Print & printer) const override {
		StaticJsonDocument<ProgramSettings::TASKREF_JSON_BUFFER_SIZE> doc;
		JsonObject object = doc.to<JsonObject>();
		encodeJSON(object);
		return serializeJsonPretty(object, Serial);
	}
};

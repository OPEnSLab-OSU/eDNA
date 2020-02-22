#pragma once
#include <KPArray.hpp>
#include <KPFoundation.hpp>

#include <ArduinoJson.h>

#include <Application/Constants.hpp>
#include <Utilities/JsonEncodableDecodable.hpp>

#include <Valve/ValveStatus.hpp>

//
// ─── SECTION VALVE OBJECT ───────────────────────────────────────────────────────
//
struct Valve : public JsonEncodable, public JsonDecodable, public Printable {
public:
	int id		  = ValveStatus::inactive;
	int status	  = ValveStatus::inactive;
	long schedule = ValveStatus::inactive;

	char group[ProgramSettings::VALVE_GROUP_LENGTH]{0};

	unsigned long flushTime		 = 0;
	unsigned long flushVolume	 = 0;
	unsigned long sampleTime	 = 0;
	unsigned long samplePressure = 0;
	unsigned long sampleVolume	 = 0;
	unsigned long dryTime		 = 0;
	unsigned long preserveTime	 = 0;

	Valve()					   = default;
	Valve(const Valve & other) = default;
	Valve & operator=(const Valve &) = default;

	explicit Valve(const JsonObjectConst & data) {
		decodeJSON(data);
	}

	const char * decoderName() const override {
		return "Valve";
	}

	const char * encoderName() const override {
		return "Valve";
	}

	void setStatus(ValveStatus vs) {
		status = vs;
	}

	void load(const char * filepath) override {
		JsonFileLoader loader;
		loader.load<ProgramSettings::VALVE_JSON_BUFFER_SIZE>(filepath, *this);
	}

	void save(const char * filepath) const override {
		JsonFileLoader loader;
		loader.save<ProgramSettings::VALVE_JSON_BUFFER_SIZE>(filepath, *this);
	}

	void decodeJSON(const JsonObjectConst & source) override {
		using namespace JsonKeys;
		// -> group
		strncpy(group, source[VALVE_GROUP], ProgramSettings::VALVE_GROUP_LENGTH);
		if (group[ProgramSettings::VALVE_GROUP_LENGTH - 1] != 0) {
			println("Warning (Valve): Group name exceeds its buffer size and will be truncated");
		}

		id			   = source[VALVE_ID];
		schedule	   = source[VALVE_SCHEDULE];
		status		   = source[VALVE_STATUS];
		flushTime	   = source[VALVE_FLUSH_TIME];
		flushVolume	   = source[VALVE_FLUSH_VOLUME];
		sampleTime	   = source[VALVE_SAMPLE_TIME];
		samplePressure = source[VALVE_SAMPLE_PRESSURE];
		sampleVolume   = source[VALVE_SAMPLE_VOLUME];
		dryTime		   = source[VALVE_DRY_TIME];
		preserveTime   = source[VALVE_PRESERVE_TIME];
	}

	bool encodeJSON(JsonObject & dest) const override {
		using namespace JsonKeys;
		return dest[VALVE_ID].set(id)
			   && dest[VALVE_GROUP].set((char *)group)
			   && dest[VALVE_SCHEDULE].set(schedule)
			   && dest[VALVE_STATUS].set(status)
			   && dest[VALVE_FLUSH_TIME].set(flushTime)
			   && dest[VALVE_FLUSH_VOLUME].set(flushVolume)
			   && dest[VALVE_SAMPLE_TIME].set(sampleTime)
			   && dest[VALVE_SAMPLE_PRESSURE].set(samplePressure)
			   && dest[VALVE_SAMPLE_VOLUME].set(sampleVolume)
			   && dest[VALVE_DRY_TIME].set(dryTime)
			   && dest[VALVE_PRESERVE_TIME].set(preserveTime);
	}

	size_t printTo(Print & printer) const override {
		StaticJsonDocument<ProgramSettings::VALVE_JSON_BUFFER_SIZE> doc;
		JsonObject object = doc.to<JsonObject>();
		encodeJSON(object);
		return serializeJsonPretty(object, Serial);
	}
};

//
// ─── SECTION VALVEREF FOR REDUCED NETWORK BANDWIDTH ─────────────────────────────
//
struct Valveref : public JsonEncodable, public JsonDecodable, public Printable {
	int id = -1;
	char group[ProgramSettings::VALVE_GROUP_LENGTH]{0};

	Valveref()						 = default;
	Valveref(const Valveref & other) = default;
	Valveref & operator=(const Valveref &) = default;

	Valveref(const Valve & valve) {
		id = valve.id;
		strcpy(group, valve.group);
	}

	explicit Valveref(const JsonObjectConst & data) {
		decodeJSON(data);
	}

	const char * decoderName() const {
		return "Valveref";
	}

	const char * encoderName() const {
		return "Valveref";
	}

	void load(const char * filepath) override {
		JsonFileLoader loader;
		loader.load<ProgramSettings::VALVE_JSON_BUFFER_SIZE>(filepath, *this);
	}

	void save(const char * filepath) const override {
		JsonFileLoader loader;
		loader.save<ProgramSettings::VALVE_JSON_BUFFER_SIZE>(filepath, *this);
	}

	void decodeJSON(const JsonObjectConst & source) override {
		using namespace JsonKeys;

		// -> group
		strncpy(group, source[VALVE_GROUP], ProgramSettings::VALVE_GROUP_LENGTH);
		if (group[ProgramSettings::VALVE_GROUP_LENGTH - 1] != 0) {
			println("Warning (Valve): Group name exceeds its allowed length and will be truncated");
		}

		id = source[VALVE_ID];
	}

	bool encodeJSON(JsonObject & dest) const override {
		using namespace JsonKeys;
		println(group);
		return dest[VALVE_ID].set(id)
			   && dest[VALVE_GROUP].set((char *)group);
	}

	size_t printTo(Print & printer) const override {
		StaticJsonDocument<ProgramSettings::VALVE_JSON_BUFFER_SIZE> doc;
		JsonObject object = doc.to<JsonObject>();
		encodeJSON(object);
		return serializeJsonPretty(object, Serial);
	}
};
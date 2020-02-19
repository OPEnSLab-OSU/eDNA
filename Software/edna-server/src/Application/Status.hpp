#pragma once
#include <KPFoundation.hpp>
#include <Application/Config.hpp>
#include <array>

#include <Utilities/JsonFileLoader.hpp>
#include <Valve/ValveStatus.hpp>
#include <Valve/ValveManager.hpp>

//
// ─── NOTE THIS OBJECT IS INTENDED TO BE READ-ONLY THAT PROVIDES SYSTEM-WIDE STATUS
//
class Status : public JsonDecodable,
			   public JsonEncodable,
			   public Printable,
			   public ValveManagerEventListner {
public:
	KPArray<int, ProgramSettings::MAX_VALVES> valves;
	int valveCurrent = -1;

	bool isFull = false;

	float pressure	  = 0;
	float temperature = 0;
	float barometric  = 0;

	float waterVolume = 0;
	float waterDepth  = 0;
	float waterFlow	  = 0;

	const char * currentStateName = nullptr;
	const char * currentTaskName  = nullptr;
	bool preventShutdown		  = false;

	Status()			   = default;
	Status(const Status &) = delete;
	Status & operator=(const Status &) = delete;

	void init(Config & config) {
		valves.resize(config.valveUpperBound + 1);
		memcpy(valves.getBuffer().data(), config.valves, sizeof(int) * valves.size());
	}

	//
	// ─── SECTION JSONDECODABLE COMPLIANCE ───────────────────────────────────────────
	//
	const char * decoderName() const override {
		return "Status";
	}

	// May be used to resume operation in future versions.
	// For now, status file is used to save valves status for next start up.
	void decodeJSON(const JsonObjectConst & source) override {
		const JsonArrayConst & source_valves = source[JsonKeys::VALVES].as<JsonArrayConst>();
		copyArray(source_valves, valves.getBuffer().data(), valves.size());
		valves.resize(source_valves.size());
	}

	// Update the content of status file
	void save(const char * filepath) const override {
		JsonFileLoader loader;
		loader.save<ProgramSettings::STATUS_JSON_BUFFER_SIZE>(filepath, *this);
	}

	//
	// ─── SECTION JSONENCODABLE COMPLIANCE ───────────────────────────────────────────
	//
	const char * encoderName() const override {
		return "Status";
	}

	bool encodeJSON(JsonObject & dest) const override {
		using namespace JsonKeys;

		JsonArray doc_valves = dest.createNestedArray(VALVES);
		copyArray(valves.getBuffer().data(), valves.size(), doc_valves);

		return dest[VALVES_COUNT].set(valves.size())
			   && dest[SENSOR_PRESSURE].set(pressure)
			   && dest[SENSOR_TEMP].set(temperature)
			   && dest[SENSOR_BARO].set(barometric)
			   && dest[SENSOR_VOLUME].set(waterVolume)
			   && dest[SENSOR_DEPTH].set(waterDepth)
			   && dest[SENSOR_FLOW].set(waterFlow)
			   && dest[STATE_CURRENT_NAME].set(currentStateName)
			   && dest[TASK_CURRENT_NAME].set(currentTaskName);
	}

	void load(const char * filepath) override {
		JsonFileLoader loader;
		loader.load<ProgramSettings::STATUS_JSON_BUFFER_SIZE>(filepath, *this);
	}

	//
	// ─── SECTION PRINTABLE COMPLIANCE ───────────────────────────────────────────────
	//
	size_t printTo(Print & printer) const override {
		StaticJsonDocument<ProgramSettings::STATUS_JSON_BUFFER_SIZE> doc;
		JsonObject object = doc.to<JsonObject>();
		encodeJSON(object);
		return serializeJsonPretty(object, printer);
	}

	//
	// ─── SECTION VALVEMANAGEREVENTLISTENER COMPLIANCE ───────────────────────────────
	//
private:
	void valvesChanged(const ValveManager & vm) override {
		for (const Valve & v : vm.valves) {
			valves[v.id] = v.status;
		}
	}

public:
	// Override_Mode_Pin is connected to an external switch which is active low
	static bool isProgrammingMode() {
		return analogRead(Override_Mode_Pin) <= 100;
	}
};
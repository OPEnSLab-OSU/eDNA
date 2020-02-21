
#pragma once

#include <KPFoundation.hpp>
#include <KPDataStoreInterface.hpp>
#include <Application/Constants.hpp>
#include <Utilities/JsonFileLoader.hpp>

#include <ArduinoJson.h>

//
// ─── SECTION CONFIG ─────────────────────────────────────────────────────────────
//
// NOTE This object is intended to be read-only and represents to actual data in the
// config file in SD card
// ────────────────────────────────────────────────────────────────────────────────
class Config : public JsonDecodable, public Printable {
public:
	int valveUpperBound;
	int valves[ProgramSettings::MAX_VALVES]{0};

	const char * configFilepath;

	char logFile[ProgramSettings::SD_FILE_NAME_LENGTH]{0};
	char statusFile[ProgramSettings::SD_FILE_NAME_LENGTH]{0};
	char taskFolder[ProgramSettings::SD_FILE_NAME_LENGTH]{0};
	char valveFolder[ProgramSettings::SD_FILE_NAME_LENGTH]{0};

public:
	Config()			   = delete;
	Config(const Config &) = delete;
	Config & operator=(const Config &) = delete;

	explicit Config(const char * configFilepath)
		: configFilepath(configFilepath) {
	}

	const char * decoderName() const override {
		return "Config";
	}

	void decodeJSON(const JsonObjectConst & source) override {
		using namespace ProgramSettings;
		using namespace ConfigKeys;

		valveUpperBound = source[VALVE_UPPER_BOUND];

		// <- valves
		JsonArrayConst config_valves = source[VALVES_FREE].as<JsonArrayConst>();
		for (int freeValveId : config_valves) {
			if (freeValveId < 0) {
				KPStringBuilder<120> message("Config: ", freeValveId, " < 0 ");
				raise(Error(message));
			}

			if (freeValveId > valveUpperBound) {
				KPStringBuilder<120> message("Config: ", freeValveId, " > ", valveUpperBound);
				raise(Error(message));
			} else {
				valves[freeValveId] = 1;
			}
		}

		strncpy(logFile, source[FILE_LOG], SD_FILE_NAME_LENGTH);
		strncpy(statusFile, source[FILE_STATUS], SD_FILE_NAME_LENGTH);
		strncpy(taskFolder, source[FOLDER_TASK], SD_FILE_NAME_LENGTH);
		strncpy(valveFolder, source[FOLDER_VALVE], SD_FILE_NAME_LENGTH);
	}

	void load(const char * filepath = nullptr) override {
		JsonFileLoader loader;
		loader.load<ProgramSettings::CONFIG_JSON_BUFFER_SIZE>(configFilepath, *this);
	}

	size_t printTo(Print & p) const override {
		using namespace ConfigKeys;
		StaticJsonDocument<ProgramSettings::CONFIG_JSON_BUFFER_SIZE> doc;
		doc[VALVE_UPPER_BOUND].set(valveUpperBound);

		// -> valves
		JsonArray doc_valves = doc.createNestedArray(VALVES_FREE);
		copyArray(valves, doc_valves);

		doc[FILE_LOG]	  = logFile;
		doc[FILE_STATUS]  = statusFile;
		doc[FOLDER_TASK]  = taskFolder;
		doc[FOLDER_VALVE] = valveFolder;

		return serializeJsonPretty(doc, Serial);
	}
};
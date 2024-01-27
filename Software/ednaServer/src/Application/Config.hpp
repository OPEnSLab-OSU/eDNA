#pragma once

#include <KPFoundation.hpp>

#include <Application/Constants.hpp>
#include <Utilities/JsonFileLoader.hpp>

//
// ──────────────────────────────────────────────────── I ──────────
//   :::::: C O N F I G : :  :   :    :     :        :          :
// ──────────────────────────────────────────────────────────────
//
// NOTE: This object is intended to be read-only and mirrors actual data in the
// config file + some userful logic
// ────────────────────────────────────────────────────────────────────────────────
class Config : public JsonDecodable, public JsonEncodable, public Printable {
public:
    bool shutdownOverride       = true;
    const char * configFilepath = nullptr;
    signed char valveUpperBound = 0;
    signed char numberOfValves  = 0;
    signed char maxPressure = 0;
    signed char cutoffPressure = 0;


    signed char valves[ProgramSettings::MAX_VALVES]        = {0};
    char logFile[ProgramSettings::SD_FILE_NAME_LENGTH]     = {0};
    char statusFile[ProgramSettings::SD_FILE_NAME_LENGTH]  = {0};
    char taskFolder[ProgramSettings::SD_FILE_NAME_LENGTH]  = {0};
    char valveFolder[ProgramSettings::SD_FILE_NAME_LENGTH] = {0};

public:
    // Config()			   = delete;
    // Config(const Config &) = delete;
    // Config & operator=(const Config &) = delete;

    explicit Config(const char * configFilepath) : configFilepath(configFilepath) {}

    static const char * decoderName() {
        return "Config";
    }

    static constexpr size_t decodingSize() {
        return ProgramSettings::CONFIG_JSON_BUFFER_SIZE;
    }

    void decodeJSON(const JsonVariant & source) override {
        using namespace ProgramSettings;
        using namespace ConfigKeys;

        valveUpperBound = source[VALVE_UPPER_BOUND];
        maxPressure = source[PRESSURE_MAX];
        cutoffPressure = source[PRESSURE_CUTOFF];
        numberOfValves  = valveUpperBound + 1;

        std::fill_n(valves, ProgramSettings::MAX_VALVES, -1);

        JsonArrayConst config_valves = source[VALVES_FREE].as<JsonArrayConst>();
        for (int freeValveId : config_valves) {
            if (freeValveId < 0) {
                KPStringBuilder<120> error("Config: ", freeValveId, " < 0 ");
                halt(TRACE, error);
            }

            if (freeValveId > valveUpperBound) {
                KPStringBuilder<120> error("Config: ", freeValveId, " > ", valveUpperBound);
                halt(TRACE, error);
            } else {
                valves[freeValveId] = 1;
            }
        }

        strncpy(logFile, source[FILE_LOG], SD_FILE_NAME_LENGTH);
        strncpy(statusFile, source[FILE_STATUS], SD_FILE_NAME_LENGTH);
        strncpy(taskFolder, source[FOLDER_TASK], SD_FILE_NAME_LENGTH);
        strncpy(valveFolder, source[FOLDER_VALVE], SD_FILE_NAME_LENGTH);
    }

#pragma region JSONENCODABLE
    static const char * encoderName() {
        return "config";
    }

    static constexpr size_t encodingSize() {
        return ProgramSettings::CONFIG_JSON_BUFFER_SIZE;
    }

    bool encodeJSON(const JsonVariant & dest) const override {
        using namespace ConfigKeys;

        JsonArray array_array = dest.createNestedArray(VALVES_FREE);
        copyArray(valves, array_array);

        return dest[VALVE_UPPER_BOUND].set(valveUpperBound) && dest[FILE_LOG].set(logFile)
               && dest[FILE_STATUS].set(statusFile) && dest[FOLDER_TASK].set(taskFolder)
               && dest[FOLDER_VALVE].set(valveFolder) && dest[PRESSURE_MAX].set(maxPressure)
               && dest[PRESSURE_CUTOFF].set(cutoffPressure);
    }
#pragma endregion
#pragma region PRINTABLE
    size_t printTo(Print & p) const override {
        using namespace ConfigKeys;
        StaticJsonDocument<encodingSize()> doc;
        encodeJSON(doc.to<JsonVariant>());

        return serializeJsonPretty(doc, Serial);
    }
#pragma endregion
};
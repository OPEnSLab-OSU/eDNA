#pragma once
#include <KPFoundation.hpp>
#include <array>

#include <ArduinoJson.h>

#include <Application/Constants.hpp>
#include <Utilities/JsonEncodableDecodable.hpp>
#include <Utilities/JsonFileLoader.hpp>

#include <Task/TaskStatus.hpp>
#include <StateControllers/NewStateController.hpp>

struct Task : public JsonEncodable,
              public JsonDecodable,
              public Printable,
              public NewStateController::Configurator {
public:
    friend class TaskManager;

    int id;
    char name[TaskSettings::NAME_LENGTH]{0};
    char notes[TaskSettings::NOTES_LENGTH]{0};

    long createdAt = 0;
    long schedule  = 0;

    int status      = TaskStatus::inactive;
    int timeBetween = 0;

    int flushTime      = 0;
    float flushVolume  = 0;
    int sampleTime     = 0;
    float sampleVolume = 0;
    int dryTime        = 0;
    int preserveTime   = 0;

    bool deleteOnCompletion = false;

    std::vector<uint8_t> valves;

public:
    int valveOffsetStart = 0;

public:
    Task()                   = default;
    Task(const Task & other) = default;
    Task & operator=(const Task &) = default;

    explicit Task(const JsonObject & data) {
        decodeJSON(data);
    }

    int getValveOffsetStart() const {
        return valveOffsetStart;
    }

    int getNumberOfValves() const {
        return valves.size();
    }

    bool isCompleted() const {
        return status == TaskStatus::completed;
    }

    /** ────────────────────────────────────────────────────────────────────────────
     *  @brief Get the Current Valve ID
     *
     *  @return int -1 if no more valve, otherwise returns the valve number
     *  ──────────────────────────────────────────────────────────────────────────── */
    int getCurrentValveId() const {
        return (getValveOffsetStart() >= getNumberOfValves()) ? -1 : valves[getValveOffsetStart()];
    }

#pragma region JSONDECODABLE
    static const char * decoderName() {
        return "Task";
    }

    static constexpr size_t decodingSize() {
        return ProgramSettings::TASK_JSON_BUFFER_SIZE;
    }

    void decodeJSON(const JsonVariant & source) override {
        using namespace TaskSettings;
        using namespace TaskKeys;

        if (source.containsKey(NAME)) {
            snprintf(name, NAME_LENGTH, "%s", source[NAME].as<char *>());
        }

        if (source.containsKey(NOTES)) {
            snprintf(notes, NOTES_LENGTH, "%s", source[NOTES].as<char *>());
        }

        if (source.containsKey(VALVES)) {
            JsonArray valve_array = source[VALVES].as<JsonArray>();
            valves.resize(valve_array.size());
            copyArray(valve_array, valves.data(), valve_array.size());
            valveOffsetStart = source[VALVES_OFFSET];
        }

        id             = source[ID];
        if(source.containsKey(CREATED_AT)){
            createdAt      = source[CREATED_AT];
        }
        if(source.containsKey(SCHEDULE)){
            schedule       = source[SCHEDULE];
        }
        status         = source[STATUS];
        flushTime      = source[FLUSH_TIME];
        flushVolume    = source[FLUSH_VOLUME];
        sampleTime     = source[SAMPLE_TIME];
        sampleVolume   = source[SAMPLE_VOLUME];
        preserveTime   = source[PRESERVE_TIME];
        timeBetween    = source[TIME_BETWEEN];
    }
#pragma endregion
#pragma region JSONENCODABLE

    static const char * encoderName() {
        return "Task";
    }

    static constexpr size_t encodingSize() {
        return ProgramSettings::TASK_JSON_BUFFER_SIZE;
    }

    bool encodeJSON(const JsonVariant & dst) const override {
        using namespace TaskKeys;
        // clang-format off
		return dst[ID].set(id) 
			&& dst[NAME].set((char *)name) 
			&& dst[NOTES].set((char *) notes)
			&& dst[STATUS].set(status) 
			&& dst[CREATED_AT].set(createdAt)
			&& dst[SCHEDULE].set(schedule) 
			&& dst[FLUSH_TIME].set(flushTime)
			&& dst[FLUSH_VOLUME].set(flushVolume)
			&& dst[SAMPLE_TIME].set(sampleTime)
			&& dst[SAMPLE_VOLUME].set(sampleVolume)
			&& dst[PRESERVE_TIME].set(preserveTime)
			&& dst[TIME_BETWEEN].set(timeBetween) 
			&& dst[VALVES_OFFSET].set(getValveOffsetStart())
			&& dst[DELETE].set(deleteOnCompletion)
			&& copyArray(valves.data(), valves.size(), dst.createNestedArray(VALVES));
	}  // clang-format on

    size_t printTo(Print & printer) const override {
        StaticJsonDocument<encodingSize()> doc;
        JsonVariant object = doc.to<JsonVariant>();
        encodeJSON(object);
        return serializeJsonPretty(doc, printer);
    }
#pragma endregion

    void operator()(NewStateController::Config & config) const {
        config.flushTime      = flushTime;
        config.sampleTime     = sampleTime;
        config.sampleVolume   = sampleVolume;
        config.preserveTime   = preserveTime;
    }
};
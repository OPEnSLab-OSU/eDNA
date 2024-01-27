#pragma once
#include <KPFoundation.hpp>
#include <array>

#include <ArduinoJson.h>

#include <Application/Constants.hpp>
#include <Utilities/JsonEncodableDecodable.hpp>
#include <Utilities/JsonFileLoader.hpp>

#include <Task/TaskStatus.hpp>
#include <StateControllers/NowTaskStateController.hpp>

struct NowTask : public JsonEncodable,
              public JsonDecodable,
              public Printable,
              public NowTaskStateController::Configurator {
public:
    friend class NowTaskManager;

    int id         = 0;
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
    int samplePressure = 0;
    int dryTime        = 0;
    int preserveTime   = 0;

    bool deleteOnCompletion = false;
    int valve = 0;
//    std::vector<uint8_t> valves;

public:
    int valveOffsetStart = 0;

public:
    NowTask() = default;
    NowTask(const NowTask & other) = default;
    NowTask & operator=(const NowTask &) = default;

    explicit NowTask(const JsonObject & data) {
        decodeJSON(data);
    }

    int getValveOffsetStart() const {
        return valveOffsetStart;
    }

    int getNumberOfValves() const {
        return 1;
        //return valves.size();
    }

    bool isCompleted() const {
        return status == TaskStatus::completed;
    }

#pragma region JSONDECODABLE
    static const char * decoderName() {
        return "NowTask";
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

        id             = source[ID];
        status         = source[STATUS];
        flushTime      = source[FLUSH_TIME];
        flushVolume    = source[FLUSH_VOLUME];
        sampleTime     = source[SAMPLE_TIME];
        sampleVolume   = source[SAMPLE_VOLUME];
        preserveTime   = source[PRESERVE_TIME];
        valve         = source[CURR_VALVE];
    }
#pragma endregion
#pragma region JSONENCODABLE

    static const char * encoderName() {
        return "NowTask";
    }

    static constexpr size_t encodingSize() {
        return ProgramSettings::TASK_JSON_BUFFER_SIZE;
    }

    bool encodeJSON(const JsonVariant & dst) const override {
        using namespace TaskKeys;
        // clang-format off
		return dst[ID].set(id)
            && dst[NAME].set((char *)name)  
			&& dst[STATUS].set(status) 
			&& dst[FLUSH_TIME].set(flushTime)
			&& dst[FLUSH_VOLUME].set(flushVolume)
			&& dst[SAMPLE_TIME].set(sampleTime)
			&& dst[SAMPLE_VOLUME].set(sampleVolume)
			&& dst[PRESERVE_TIME].set(preserveTime)
			&& dst[CURR_VALVE].set(valve);
	}  // clang-format on

    size_t printTo(Print & printer) const override {
        StaticJsonDocument<encodingSize()> doc;
        JsonVariant object = doc.to<JsonVariant>();
        encodeJSON(object);
        return serializeJsonPretty(doc, printer);
    }
#pragma endregion

    void operator()(NowTaskStateController::Config & config) const {
        config.flushTime      = flushTime;
        config.sampleTime     = sampleTime;
        config.sampleVolume   = sampleVolume;
        config.preserveTime   = preserveTime;
    }

    void operator()(NewStateController::Config & config) const {
        config.flushTime      = flushTime;
        config.sampleTime     = sampleTime;
        config.sampleVolume   = sampleVolume;
        config.preserveTime   = preserveTime;
    }
};
#pragma once
#include <KPFoundation.hpp>

#include <ArduinoJson.h>
#include <Application/Constants.hpp>
#include <Utilities/JsonEncodableDecodable.hpp>
#include <Valve/ValveStatus.hpp>

//
// ────────────────────────────────────────────────── I ──────────
//   :::::: V A L V E : :  :   :    :     :        :          :
// ────────────────────────────────────────────────────────────
//

struct Valve : public JsonEncodable, public JsonDecodable, public Printable {
public:
    int id     = ValveStatus::unavailable;
    int status = ValveStatus::unavailable;
    char group[ProgramSettings::VALVE_GROUP_LENGTH]{0};

    Valve()                    = default;
    Valve(const Valve & other) = default;
    Valve & operator=(const Valve &) = default;

    /** ────────────────────────────────────────────────────────────────────────────
     *  Explicit construction of a new Valve object from JSON data
     *
     *  @param data Data in form of ArduinoJson's JsonObject
     *  ──────────────────────────────────────────────────────────────────────────── */
    explicit Valve(const JsonObject & data) {
        decodeJSON(data);
    }

    void setStatus(ValveStatus status) {
        this->status = status;
    }

#pragma region JSONDECODABLE
    static const char * decoderName() {
        return "Valve";
    }

    static constexpr size_t decodingSize() {
        return ProgramSettings::VALVE_JSON_BUFFER_SIZE;
    }

    void decodeJSON(const JsonVariant & src) override {
        using namespace ValveKeys;
        // -> group
        strncpy(group, src[GROUP], ProgramSettings::VALVE_GROUP_LENGTH);
        if (group[ProgramSettings::VALVE_GROUP_LENGTH - 1] != 0) {
            println("Warning (Valve): Group name exceeds its buffer size and will be truncated");
        }

        status = src[STATUS];
    }
#pragma endregion
#pragma region JSONENCODABLE
    const char * encoderName() const {
        return "Valve";
    }

    static constexpr size_t encodingSize() {
        return ProgramSettings::VALVE_JSON_BUFFER_SIZE;
    }

    bool encodeJSON(const JsonVariant & dst) const override {
        using namespace ValveKeys;
        // clang-format off
		return dst[ID].set(id)
			   && dst[GROUP].set((char *) group)
			   && dst[STATUS].set(status);
	}  // clang-format on

#pragma endregion
#pragma region PRINTABLE
    size_t printTo(Print & printer) const override {
        StaticJsonDocument<encodingSize()> doc;
        JsonVariant object = doc.to<JsonVariant>();
        encodeJSON(object);
        return serializeJsonPretty(object, Serial);
    }
#pragma endregion
};
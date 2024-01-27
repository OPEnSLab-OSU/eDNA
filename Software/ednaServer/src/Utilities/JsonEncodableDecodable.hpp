#pragma once

#include <KPFoundation.hpp>
#include <ArduinoJson.h>

// ────────────────────────────────────────────────────────────────────────────────
// ─── SECTION  INTERFACE FOR CUSTOM JSON DECODING OBJECT ─────────────────────────
// ────────────────────────────────────────────────────────────────────────────────
class JsonDecodable {
public:
    static const char * decoderName() {
        return "Unnamed";
    }

    static constexpr size_t decodingSize();
    virtual void decodeJSON(const JsonVariant & source) = 0;
};

inline void decodeJSON(JsonDecodable & decoder, const JsonVariant & source) {
    decoder.decodeJSON(source);
};

// ────────────────────────────────────────────────────────────────────────────────
// ─── SECTION  INTERFACE FOR CUSTOM JSON ENCODING OBJECT ─────────────────────────
// ────────────────────────────────────────────────────────────────────────────────
class JsonEncodable {
public:
    static const char * encoderName() {
        return "Unnamed";
    }

    static const size_t encodingSize();
    virtual bool encodeJSON(const JsonVariant & dest) const = 0;
};

inline void encodeJSON(JsonEncodable & encoder, const JsonVariant & dest) {
    encoder.encodeJSON(dest);
};

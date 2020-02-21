#pragma once

#include <KPFoundation.hpp>
#include <ArduinoJson.h>

//
// ─── SECTION  INTERFACE FOR CUSTOM JSON DECODING OBJECT ────────────────────────────
//
class JsonDecodable {
public:
	virtual const char * decoderName() const {
		return "Unnamed";
	}

	virtual void decodeJSON(const JsonObjectConst & source) = 0;
	virtual void load(const char * filepath) {
		raise(Error("JsonDecodable load needs override"));
	}
};

//
// ─── SECTION  INTERFACE FOR CUSTOM JSON ENCODING OBJECT ────────────────────────────
//
class JsonEncodable {
public:
	virtual const char * encoderName() const {
		return "Unnamed";
	}

	virtual bool encodeJSON(JsonObject & dest) const = 0;
	virtual void save(const char * filepath) const {
		raise(Error("JsonEncodable save needs override"));
	}
};

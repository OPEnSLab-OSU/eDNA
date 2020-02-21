#pragma once
#include <StreamUtils.h>
#include <Utilities/FileLoader.hpp>
#include <Utilities/JsonEncodableDecodable.hpp>

class JsonFileLoader : public FileLoader {
public:
	template <size_t buffer_size>
	void load(const char * filepath, JsonDecodable & decoder) const {
		unsigned long start = millis();

		// raise error if file doesn't exist to notify the user
		File file = SD.open(filepath, FILE_READ);
		if (!file) {
			KPStringBuilder<120> message("JsonFileLoader: ", filepath, " doesn't exist");
			raise(Error(message));
		}

		// skip empty file
		if (file.size() == 0) {
			println("JsonFileLoader: ", filepath, " is empty");
			return;
		}

		// deserialize file to JSON document
		StaticJsonDocument<buffer_size> doc;
		const DeserializationError error = deserializeJson(doc, file);
		file.close();

		// handle deserialization error
		switch (error.code()) {
		case DeserializationError::Ok:
			break;
		case DeserializationError::NoMemory: {
			KPStringBuilder<120> message("Decoder (", decoder.decoderName(), "): ", filepath, " size exeecds the buffer limit.");
			raise(Error(message));
		} break;
		default:
			KPStringBuilder<200> message("Decoder (", decoder.decoderName(), "): ", filepath, " deserialization failed -> ", error.c_str());
			raise(Error(message));
		}

		// the status message
		println();
		println("Finished loading from ", filepath, " in ", millis() - start, " ms");
		println("Json size: ", doc.memoryUsage(), " bytes");
		decoder.decodeJSON(doc.template as<JsonObjectConst>());
	}

	template <size_t buffer_size>
	void save(const char * filepath, const JsonEncodable & encoder) const {
		// call the encoder function
		StaticJsonDocument<buffer_size> doc;
		JsonObject dest = doc.template to<JsonObject>();
		if (!encoder.encodeJSON(dest)) {
			KPStringBuilder<120> message("Encoder (", encoder.encoderName(), "): JSON object size exceeds the buffer limit.");
			raise(Error(message));
		}

		// timestamp
		unsigned long start = millis();

		// serialize JSON document to file
		File file = SD.open(filepath, FILE_WRITE | O_TRUNC);
		serializeJson(dest, file);
		file.close();

		// the status message
		println();
		println("Finished writing to ", filepath, " in ", millis() - start, " ms");
		println("Json size: ", doc.memoryUsage(), " bytes");
	}
};
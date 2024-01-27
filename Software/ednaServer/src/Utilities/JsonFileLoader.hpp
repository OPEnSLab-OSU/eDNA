#pragma once
#include <StreamUtils.h>
#include <Utilities/FileLoader.hpp>
#include <Utilities/JsonEncodableDecodable.hpp>

class JsonFileLoader : public FileLoader {
public:
    template <size_t size>
    void load(const char * filepath, StaticJsonDocument<size> & dst) {
        File file = SD.open(filepath, FILE_READ);
        if (!file) {
            KPStringBuilder<120> message("JsonFileLoader: ", filepath, " doesn't exist");
            // println(Error(message));
            return;
        }

        // skip empty file
        if (file.size() == 0) {
            println("JsonFileLoader: ", filepath, " is empty");
            return;
        }

        deserializeJson(dst, file);
    }

    template <typename Decoder>
    void load(const char * filepath, Decoder & decoder) const {
        unsigned long start = millis();

        // raise error if file doesn't exist to notify the user
        if (!SD.begin(HardwarePins::SD_CARD)) {
            println("Not ready");
        };

        File file = SD.open(filepath, FILE_READ);
        if (!file) {
            KPStringBuilder<120> message("JsonFileLoader: ", filepath, " doesn't exist");
            println(message);
            file.close();
            return;
        }

        // skip empty file
        if (file.size() == 0) {
            println("JsonFileLoader: ", filepath, " is empty");
            file.close();
            return;
        }

        // deserialize file to JSON document
        StaticJsonDocument<Decoder::decodingSize()> doc;
        const DeserializationError error = deserializeJson(doc, file);
        file.close();

        // handle deserialization error
        switch (error.code()) {
        case DeserializationError::Ok:
            break;
        case DeserializationError::NoMemory: {
            KPStringBuilder<120> message(
                decoder.decoderName(), " decoder: size exeecds the buffer limit whlie decoding ",
                filepath);
            halt(TRACE, message);
        } break;
        default:
            KPStringBuilder<120> message(
                decoder.decoderName(), " decoder: ", error.c_str(), " while decoding ", filepath);
            halt(TRACE, message);
        }

        println();
        println("Finished loading from ", filepath, " in ", millis() - start, " ms");
        println("Json size: ", doc.memoryUsage(), " bytes");
        decoder.decodeJSON(doc.template as<JsonVariant>());
    }

    template <typename Encoder>
    void save(const char * filepath, const Encoder & encoder) const {
        // call the encoder function
        StaticJsonDocument<Encoder::encodingSize()> doc;
        JsonVariant dest = doc.template to<JsonVariant>();
        if (!encoder.encodeJSON(dest)) {
            KPStringBuilder<120> message(
                "Encoder (", encoder.encoderName(),
                "): JSON object size exceeds the buffer limit.");
            halt(TRACE, message);
        }

        save(filepath, doc);
    }

    template <size_t size>
    void save(const char * filepath, StaticJsonDocument<size> & src) const {
        // timestamp
        unsigned long start = millis();

        if (!SD.begin(HardwarePins::SD_CARD)) {
            println("Not ready");
        };

        // serialize JSON document to file
        File file = SD.open(filepath, O_RDWR | O_CREAT | O_TRUNC);
        serializeJson(src, file);
        file.close();

        println();
        println("Finished writing to ", filepath, " in ", millis() - start, " ms");
        println("Json size: ", src.memoryUsage(), " bytes");
    }
};
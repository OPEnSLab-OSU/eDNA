#pragma once
#include "KPFoundation.hpp"
#include "KPComponent.hpp"
#include "ArduinoJson.h"
#include "SPI.h"
#include "SD.h"
#include "Time.h"

class KPSDCard: public KPComponent {
public:	
	bool checkForConnection() override {
		return SD.begin(SDCard_Pin);
	}

	int loadContentsOfFile(const char * filename, char * buffer = nullptr) {
		if  (filename == nullptr) {
			return 0;
		}

		File file = SD.open(filename);
		if (!file) {
			return 0;
		}

		char message[128] = {0};
		sprintf(message, "%lu bytes loaded from %s", file.size(), filename);
		Serial.println(message);

		const int bufferSize = file.size();
		if (!buffer) {
			buffer = new char[bufferSize + 1];
		}

		buffer[bufferSize] = 0;
		int size = file.read(buffer, bufferSize);
		file.close();
		return size;
	}

	int loadContentsOfFile(const char * filename, char * buffer, const int bufferSize) {
		if (bufferSize <= 0 || buffer == nullptr || filename == nullptr) {
			return 0;
		}

		static int position = 0;
		static const char * prevFilename = nullptr;
		if (filename != prevFilename) {
			position = 0;
		}

		prevFilename = filename;

		File file = SD.open(filename);
		if (!file) {
			position = 0;
			return 0;
		}

		// Calculate how many bytes are remaining to be read
		int bytesRemaining = file.size() - position;
		Serial.print("Bytes Remaining: ");
		Serial.println(bytesRemaining);
		if (bytesRemaining <= 0) {
			position = 0;
			Serial.println("EOF");
			file.close();
			return 0;
		}

		// Copy 
		int size = constrain(bytesRemaining - 1, 0, bufferSize - 1);
		buffer[size] = 0;
		file.seek(position);
		file.read(buffer, size);
		file.close();

		position += size;		
		return size;
	}

	bool loadJsonFromFile(char * filepath, JsonDocument & doc) {
        if (!SD.exists(filepath)) {
            return false;
        }

        File file = SD.open(filepath);
        deserializeJson(doc, file);
		file.close();
        return true;
    }

	bool loadConfigurationFromCurrentSession() {
		KPStatus & s = KPStatus::current();

		// Filepaths
		pathToLastestSession(s.sessionDirectory);
		sprintf(s.logFilepath, "%s%s", s.sessionDirectory, "logs.csv");
		sprintf(s.statusFilepath, "%s%s", s.sessionDirectory, "status.js");

		// Check if status file exist
		char * statusFilepath = s.statusFilepath;
		if (!SD.exists(statusFilepath)) {
			return false;
		}

		// Load status file
		File statusFile = SD.open(statusFilepath, FILE_READ);
		StaticJsonDocument<1000> doc;
		deserializeJson(doc, statusFile);
		statusFile.close();

		// Valve Configuration
		JsonArray valves = doc["freeValves"];
		for (JsonVariant v : valves) {
			s.valves[v.as<int>()] = true;
			Serial.println(v.as<int>());
		}
		
		return true;
	}

	void makeDirectory(char * path) {
		if (SD.mkdir(path)) {
			Serial.println("Directory created");
		} else {
			Serial.println("Cannot create directory");
		}
	}


	void pathToLastestSession(char * buffer) {
		const char * sessionsFolderPath = "sessions/";
		File sessionsDir = SD.open(sessionsFolderPath);
		if (!sessionsDir || !sessionsDir.isDirectory()) {
			Serial.print("Invalid folder structure: ");
			Serial.println(sessionsFolderPath);
			sessionsDir.close();
			return;
		} else {
			sessionsDir.close();
		}

		// Get the lastest session
		int id = -1;
		while (true) {
			sprintf(buffer, "%sS%0*d", sessionsFolderPath, 4, id + 1);
			if (SD.exists(buffer)) {
				id++;
			} else {
				break;
			}
		}

		if (id == -1) {
			buffer = nullptr;
		} else {
			sprintf(buffer, "%sS%0*d%s", sessionsFolderPath, 4, id, "/");
		}
	}
};
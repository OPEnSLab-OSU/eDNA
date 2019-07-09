#include "OPComponent.hpp"
#include "Status.hpp"
#include "SPI.h"
#include "SD.h"
#include "Time.h"
#define cardSelect 10

class OPSDCard: public OPComponent {
public:
	using OPComponent::OPComponent;
	void setup() override {
		if (!SD.begin(cardSelect)) {
			while (true) {
				Serial.println("SD Card Initialization Failed!");
				delay(3000);
			}
		}
	}

	char * loadContentsOfFile(const char * filename, char * buffer = nullptr) {
		if  (filename == nullptr) {
			return nullptr;
		}

		File file = SD.open(filename);
		if (!file) {
			return nullptr;
		}

		char message[128] = {0};
		sprintf(message, "%lu bytes loaded from %s", file.size(), filename);
		Serial.println(message);

		const int size = file.size();
		if (!buffer) {
			buffer = new char[size + 1];
		}

		buffer[size] = 0;
		file.read(buffer, size);
		file.close();
		return buffer;
		
	}

	char * loadContentsOfFile(const char * filename, char * buffer, const int size) {
		if (size <= 0 || buffer == nullptr || filename == nullptr) {
			return nullptr;
		}

		static int position = 0;
		File file = SD.open(filename);
		if (!file) {
			return nullptr;
		}
	
		int bytesRemaining = file.size() - position;
		Serial.print("Bytes Remaining: ");
		Serial.println(bytesRemaining);
		if (bytesRemaining <= 0) {
			position = 0;
			Serial.println("EOF");
			file.close();
			return nullptr;
		}

		file.seek(position);
		if (size <= bytesRemaining) {
			buffer[size - 1] = 0;
			position += size - 1;
			file.read(buffer, size - 1);
		} else {
			buffer[bytesRemaining] = 0;
			position += bytesRemaining;
			file.read(buffer, bytesRemaining);
		}

		file.close();
		return buffer;
	}

	void makeDirectory(char * path) {
		if (SD.mkdir(path)) {
			Serial.println("Directory created");
		} else {
			Serial.println("Cannot create directory");
		}
	}

	void logCurrentStatus() {
		char * filepath = Status::current().filepath;
		if (!SD.exists(filepath)) {
			File file = SD.open(filepath, FILE_WRITE);
			file.println("UTC Time, State, Temperature, Flow, Pressure, Battery, Water Depth, Barometic");
			file.close();
		} 

		File file = SD.open(filepath, FILE_WRITE);
		if (file) {
			Status & s = Status::current();

			TimeElements tm;
			breakTime(now(), tm);

			char temp[6];
			sprintf(temp, "%.2f", Status::current().temperature);

			char message[1024];
			const char * formatter = "%l, %s, %.2f";
			sprintf(message, formatter, now(), s.stateName, s.temperature);

			file.println(message);
			file.close();
			Serial.println("Wrote to file");
		} else {
			Serial.println("File doesn't exist");
		}
	}
};
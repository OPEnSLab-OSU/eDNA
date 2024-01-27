#pragma once
#include <SD.h>

#include <KPDataStoreInterface.hpp>
#include <KPFoundation.hpp>

class KPFileLoader : public KPDataStoreInterface {
private:
	int pin;

public:
	KPFileLoader(const char * name, int pin) : KPDataStoreInterface(name), pin(pin) {}

	void setup() override {
		SD.begin(pin);
	}

	template <size_t N>
	int loadContentOfFile(const char * filepath, char (&buffer)[N],
						  int * charsRemaining = nullptr) {
		return loadContentOfFile(filepath, buffer, N, charsRemaining);
	}

	int loadContentOfFile(const char * filepath, void * buffer, size_t bufferSize,
						  int * charsRemaining = nullptr) {
		return loadContentOfFile(filepath, buffer, bufferSize, charsRemaining);
	}

	int loadContentOfFile(const char * filepath, char * buffer, size_t bufferSize,
						  int * charsRemaining = nullptr) override {
		if (bufferSize <= 0 || buffer == nullptr || filepath == nullptr) {
			println(TRACE, "Invalid arguments: ", bufferSize, " ", buffer, " ", filepath);
			return -1;
		}

		// Keep track of the position in the file
		static size_t position			 = 0;
		static const char * prevFilepath = nullptr;
		if (prevFilepath == nullptr || strcmp(filepath, prevFilepath) != 0) {
			position = 0;
		}

		prevFilepath = filepath;

		File file = SD.open(filepath, FILE_READ);
		if (!file) {
			println("File not found: ", filepath);
			position = 0;
			return -1;
		} else {
			println("File found: ", file.name());
			println("Loading content of file: ", filepath);
		}

		// Calculate how many bytes are remaining to be read
		size_t bytesRemaining = file.size() - position;
		println("Bytes Remaining: ", bytesRemaining);
		if (bytesRemaining <= 0) {
			position = 0;
			println("EOF");
			file.close();
			return 0;
		}

		// Copy
		int size	 = constrain(bytesRemaining, 0, bufferSize - 1);
		buffer[size] = 0;
		file.seek(position);
		file.read(buffer, size);

		position += size;

		if (charsRemaining) {
			*charsRemaining = file.size();
		}
		file.close();
		return size;
	}

	int saveContentToFile(const char * filepath, char * buffer, size_t bufferSize,
						  bool replaceContent = false) override {
		File file = SD.open(filepath, FILE_WRITE);
		if (replaceContent)
			file.seek(0);
		file.write(buffer, bufferSize);
		file.close();
		return bufferSize;
	}
};

#pragma once
#include <KPFoundation.hpp>
#include <SD.h>

class FileLoader {
public:
	bool createDirectoryIfNeeded(const char * dir) {
		File folder = SD.open(dir, FILE_READ);
		if (folder && !folder.isDirectory()) {
			raise(Error("FileLoader: path already exists but not directory please remove the file"));
		}

		if (folder && folder.isDirectory()) {
			return true;
		}

		if (!folder) {
			bool success = SD.mkdir(dir);
			print("FileLoader: creating directory...");
			println(success ? "success" : "failed");
			return success;
		} else {
			return true;
		}
	}
};
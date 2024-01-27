#pragma once
#include <KPFoundation.hpp>
#include <SD.h>

class FileLoader {
public:
    bool createDirectoryIfNeeded(const char * dir) {
        File folder = SD.open(dir, FILE_READ);
        if (folder) {
            if (folder.isDirectory()) {
                folder.close();
                return true;
            }

            halt(TRACE, "Directory is a file. Please remove it.");
        }

        // folder doesn't exist
        print("FileLoader: ", dir, " directory doesn't exist. Creating...");
        bool success = SD.mkdir(dir);
        println(success ? "success" : "failed");
        folder.close();
        return success;
    }
};
#ifndef UNIT_TEST
#include <KPApplicationRuntime.hpp>

#include <Application/Application.hpp>

void printDirectory(File dir, int numTabs) {
	while (true) {
		File entry = dir.openNextFile();
		if (!entry) {
			// no more files
			break;
		}
		for (uint8_t i = 0; i < numTabs; i++) {
			print('\t');
		}
		print(entry.name());
		if (entry.isDirectory()) {
			println("/");
			printDirectory(entry, numTabs + 1);
		} else {
			// files have sizes, directories do not
			print("\t\t");
			println(entry.size(), DEC);
		}
		entry.close();
	}
}

Application app;

void setup() {
	Runtime::setInitialAppController(app);
}

void loop() {
	Runtime::update();
}
#endif
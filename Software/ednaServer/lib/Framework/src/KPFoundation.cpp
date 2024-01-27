#include "KPFoundation.hpp"

Verbosity PrintConfig::printVerbose = PrintConfig::defaultPrintVerbose;
Stream * errorStream				= nullptr;

size_t free_ram() {
	char stack_dummy;
	return &stack_dummy - sbrk(0);
}

size_t printFreeRam() {
	return println("Free Memory: ", free_ram());
}

int strcmpi(const char * left, const char * right) {
	for (int i = 0;; i++) {
		int l = left[i], r = right[i];
		if (l == 0 && r == 0) {
			return 0;
		}

		if (toupper(l) != toupper(r)) {
			return l - r;
		}
	}
}

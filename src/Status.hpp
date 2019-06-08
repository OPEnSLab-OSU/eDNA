#pragma once
#include "Foundation.hpp"

struct Status {
	int startValve = 12;
	int currentValve = 12;
	int endValve = 14;
	int maxValve = 23;

	time_t samplingInteval = 15;

	float pressure;
	float temperature;

	float waterVolume;
	float waterDepth;
	float waterFlow;

	bool programmingMode = true;
	bool isFull = false;

	bool isProgrammingMode() {
		return programmingMode;
	}
};


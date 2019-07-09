#pragma once
#include "ArduinoJson.h"

/**
 * Describes plain old data for interfacing between components
 */
struct Status {
	// Valve Status
	int startValve = 12;
	int currentValve = 12;
	int endValve = 23;
	int maxValve = 23;

	time_t samplingInteval = 15;

	// Pressure Sensor
	float pressure;
	float temperature;

	// Water Values
	float waterVolume;
	float waterDepth;
	float waterFlow;

	bool isProgrammingMode = true;
	bool isFull = false;

	// SD Card
	char * directory = nullptr;
	char * filename = nullptr;
	char * filepath = nullptr;

	// State
	const char * stateName = nullptr;
	int stateCount = 0;

	// Shared
	static Status & current() {
		static Status s;
		return s;
	}
};


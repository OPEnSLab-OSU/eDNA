#pragma once
#include "Arduino.h"
#include "ArduinoJson.h"
#include "Definitions.hpp"   

class OPSystem;
enum class StateIdentifier;

class State {
public:
	StateIdentifier id;
	const char * name = nullptr;
	virtual void loadFrom(JsonDocument & doc) {}
	virtual void saveTo(JsonDocument & doc) {}
	virtual void update(OPSystem & system) {}
};

class IdleState : public State {};

class StateNode {
public:
	State * state = nullptr;

	int count = 0;
	unsigned long startTime = 0;

	void begin() {
		startTime = millis();
		count = 0;
	}

	void update(OPSystem & system) {
		state->update(system);
		count++;
	}
};
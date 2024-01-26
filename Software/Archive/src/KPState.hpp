#pragma once
#include "KPFoundation.hpp"
#include "ArduinoJson.h"

class KPSystem;
class KPStateMachine;
enum class KPStateIdentifier;

class KPState {
private:
    friend class KPStateMachine;
    int count = 0;
    unsigned long startTime = 0;

    void begin() {
        startTime = millis();
        count = 0;
		KPStatus::current().stateName = name;
    }

    void updateState(KPStateMachine & machine) {
        update(machine);
        count++;
    }

public:
    KPStateIdentifier id;
    const char * name = nullptr;

    unsigned long timeLimit = 0;
    float pressureLimit = 0;
    float volumeLimit = 0;

    unsigned long timeMax = MinsToSecs(20);
    float pressureMax = 15;
    float volumeMax = 5000;

    unsigned long timeSinceLastTransition() {
        return (unsigned long) (millis() - startTime) / 1000;
    }

    bool isInitialTransition() {
        return count == 0;
    }
	
    virtual void update(KPStateMachine & machine) {}
};

class IdleState : public KPState {};

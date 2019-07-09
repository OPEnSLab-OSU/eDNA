#pragma once
#include "Arduino.h"
#include "Array.hpp"
#include "LinkedList.hpp"
#include "WiFi101.h"

#include "OPComponent.hpp"

using VoidFunctionPointer = void(*)();
using BoolFunctionPointer = bool(*)();

enum class State;

struct StateNode {
    State state;
	const char * name;
    VoidFunctionPointer callback = nullptr;
    unsigned long startTime = 0;
    int count = 0;

	StateNode() {}
	StateNode(State state, const char name[]) : state(state), name(name) {}
    
    void begin() {
        startTime = millis();
        count = 0;
    }
    
    void update() {
        callback();
        count++;
    }
};

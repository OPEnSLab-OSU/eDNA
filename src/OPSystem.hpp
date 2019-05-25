//
//  OPSystem.hpp
//  OPEnSamplerFramework
//
//  Created by Kawin on 2/10/19.
//  Copyright © 2019 Kawin. All rights reserved.
//

#pragma once
#include "Foundation.hpp"
#include "OPComponent.hpp"
#include "Array.hpp"
#include "OPTaskScheduler.hpp"

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__
 
int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}

using VoidFunctionPointer = void(*)();

enum class State;
struct StateNode {
    State state;
    VoidFunctionPointer callback = nullptr;
    time_t startTime = 0;
    int count = 0;

	StateNode() {}
	StateNode(State state) : state(state) {}
    
    void begin() {
        startTime = millis();
        count = 0;
    }
    
    void update() {
        callback();
        count++;
    }
};


class OPSystem {
private:
    Array<StateNode> states;
    StateNode * currentState = nullptr;
    
    VoidFunctionPointer _setup_client = nullptr;
    VoidFunctionPointer _did_setup_client = nullptr;
    VoidFunctionPointer _loop_client  = nullptr;
    
    bool transitionPending = false;
public:
    Array<OPComponent *> components;
    OPTaskScheduler scheduler;
    
    OPSystem(VoidFunctionPointer _setup, VoidFunctionPointer _did_setup, VoidFunctionPointer _loop) : states(), scheduler() {
        this->_setup_client = _setup;
        this->_did_setup_client = _did_setup;
        this->_loop_client = _loop;
    };
    
    State getCurrentState() {
        return currentState->state;
    }
    
    void registerState(State state, VoidFunctionPointer callback) {
        StateNode node(state);
        node.callback = callback;
        states.append(node);
    }
    
    void transitionTo(State state) {
        for (int i = 0; i < states.size; i++) {
            StateNode & node = states.get(i);
            if (node.state == state) {
                transitionPending = true;
                currentState = &node;
            }
        }
        
        if (transitionPending == false) {
            // cout << "State Unreachable" << endl;
        }
    }

	bool isInitialTransition() {
		return callCountSinceLastTransition() == 0;
	}
    
    bool isTransitioning() {
        return transitionPending;
    }
    
    time_t timeSinceLastTransition() {
        return millis() - currentState->startTime;
    }
    
    int callCountSinceLastTransition() {
        return currentState->count;
    }
    
    void _setup() {
        _setup_client();
        for (int i = 0; i < components.size; i++) {
            components.get(i) ->setup();
        }
        _did_setup_client();
    }
    
    void _update_state() {
        if (currentState == nullptr) {
            return;
        }
        
        if (transitionPending) {
            currentState->begin();
            transitionPending = false;
        }
        
        currentState->update();
    }
    
    void _update() {
        _update_state();
        scheduler.update();
        _loop_client();
        for (int i = 0; i < components.size; i++) {
            components.get(i)->update();
        }
    }
};

extern OPSystem app;

void run(const OPTask & task) {
    app.scheduler.append(task);
}

void runForever(OPTask & task) {
    task.repeat = -1;
    app.scheduler.append(task);
}

bool containsTask(const String & name) {
    return app.scheduler.contains(name);
}

void setTimeout(long ms, VoidFunctionPointer callback) {
    OPTask task;
    task.wait(ms, callback);
    run(task);
}

void addComponent(OPComponent * c) {
    app.components.append(c);
}

void setup() {
    app._setup();
}

void loop() {
    app._update();
}
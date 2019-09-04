//
//  OPAsync.hpp
//  eDNA
//
//  Created by Kawin on 2/10/19.
//  Copyright © 2019 Kawin. All rights reserved.
//

#pragma once
#include "Arduino.h"
#include "KPList.hpp"

using VoidFunctionPointer = void(*)();
using BoolFunctionPointer = bool(*)();

struct KPAction {
    unsigned long start = 0;
    unsigned long delay = 0;
    int repeat = 1;
    
    int arg = 0;
    
    VoidFunctionPointer callback = nullptr;
    BoolFunctionPointer condition = nullptr;
};

/*
    A linkedlist where each node is an action. 
	Useful for making sequential async actions.
*/
class KPSequentialAction : public List<KPAction> {
private:
    int actionIndex = 0;
    int actionRepeatCounter = 0;

    friend class OPTaskScheduler;
    friend class OPSystem;
public:
    const char * name = nullptr;
    int repeat = 1;
public:
    
    // KPSequentialAction() : List<KPAction>() {}
    KPSequentialAction(const char * name = nullptr) : List<KPAction>(), name(name) {}
    KPSequentialAction(const KPSequentialAction & obj) : List(obj) {
        name = obj.name;
        repeat = obj.repeat;
        actionRepeatCounter = obj.actionRepeatCounter;
        actionIndex = obj.actionIndex;
    }

    // Wait for specified time in ms before callback is executed
    KPSequentialAction & wait(unsigned long ms, VoidFunctionPointer callback) {
        KPAction action;
        action.delay = ms;
        action.callback = callback;
        append(action);
        return * this;
    }
    
    KPSequentialAction & attach(int x) {
        KPAction & current = tail->prev->data;
        current.arg = x;
        return * this;
    }

    // Execute the callback immediately in the pipeline
    KPSequentialAction & now(VoidFunctionPointer callback) {
        KPAction action;
        action.delay = 0;
        action.callback = callback;
        append(action);
        return * this;
    }

    // Wrapper for better API
    KPSequentialAction & completion(VoidFunctionPointer callback) {
        return now(callback);
    }

    // Create an action with condition that can be used to lock the execution queue
    KPSequentialAction & condition(BoolFunctionPointer condition) {
        KPAction action;
        action.condition = condition;
        append(action);
        return * this;
    }
    
    // Change how many times to repeat the current action for
    KPSequentialAction & repeatFor(int times) {
        KPAction & current = tail->prev->data;
        current.repeat = times;
        return * this;
    }
    
    void nextAction() {
        actionIndex = (actionIndex + 1) % size;
        actionRepeatCounter = 0;
        
        if (actionIndex == 0) {
            repeat--;
        }

		// Serial.println("Next Action");
    }

    void update() {
        if (repeat == 0 || isEmpty()) {
            return;
        }
        
        Node * current = search(actionIndex);
        KPAction & action = current->data;
    
        if (action.condition && !action.condition()) {
            return;
        }
        
        if (action.condition && action.condition()) {
            nextAction();
            return;
        }
        
        if (actionRepeatCounter == 0) {
            actionRepeatCounter++;
            action.start = millis();
        }
        
        if ((unsigned long) (millis() - action.start) >= action.delay) {
            action.callback();
            
            if (actionRepeatCounter >= action.repeat) {
                nextAction();
            } else {
                action.start = millis();
                actionRepeatCounter++;
            }
        }
    }
};

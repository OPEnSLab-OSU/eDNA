//
//  OPSystem.hpp
//  OPEnSamplerFramework
//
//  Created by Kawin on 2/10/19.
//  Copyright © 2019 Kawin. All rights reserved.
//

#pragma once
#include "Array.hpp"
#include "List.hpp"
#include "Definitions.hpp"
#include "OPState.hpp"
#include "OPTaskScheduler.hpp"
#include "OPComponent.hpp"
#include "Status.hpp"

class OPSystem {
private:
    Array<StateNode *> stateNodes;
    StateNode * currentStateNode = nullptr;

    VoidFunctionPointer client_prepare = nullptr;
    VoidFunctionPointer client_ready = nullptr;
    VoidFunctionPointer client_loop = nullptr;

    bool transitionPending = false;

public:
    Array<OPComponent *> components;
    OPTaskScheduler scheduler;

    OPSystem(VoidFunctionPointer _preare, VoidFunctionPointer _ready, VoidFunctionPointer _loop);

    // State Machine
    void state_update();
    StateNode * getCurrentState();
    const char * getCurrentStateName();

	bool isStateRegistered(StateIdentifier id) {
		for (int i = 0; i < stateNodes.size; i++) {
			StateNode * node = stateNodes.get(i);
			if (node->state->id == id){
				return true;
			}
		}

		return false;
	}

	template<typename T = IdleState> StateNode * registerState(StateIdentifier id, const char * name) {
		if (isStateRegistered(id)) {
			return nullptr;
		}

		StateNode * node = new StateNode();
		node->state = new T();
		node->state->id = id;
		node->state->name = name;
		stateNodes.append(node);
		Status::current().stateCount++;
		return node;
	}

	State & getStateForIdentifier(StateIdentifier id) {
		for (int i = 0; i < stateNodes.size; i++) {
			StateNode * node = stateNodes.get(i);
			if (node->state->id == id) {
				return *(node->state);
			}
		}

		while (true) {
			delay(2000);
			Serial.println("State Identifier has not been registered");
		}
	}
	
	void transitionTo(StateIdentifier id) {
		for (int i = 0; i < stateNodes.size; i++) {
			StateNode * node = stateNodes.get(i);
			if (node->state->id == id) {
				transitionPending = true;
				currentStateNode = node;
				Status::current().stateName = node->state->name;
				return;
			}
		}

		while (true) {
			Serial.print("State Unreachable:");
			Serial.println(static_cast<int>(id));
			delay(3000);
		}
	}

    bool isInitialTransition();
    bool isPendingState();
    unsigned long timeSinceLastTransition();
    int callCountSinceLastTransition();

    // Components
    void addComponent(OPComponent * c);
    OPComponent * getComponent(const char * name);

    void _setup();
    void _update();
};
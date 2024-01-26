#pragma once
#include "KPFoundation.hpp"
#include "KPState.hpp"
#include "Time.h"

class KPStateMachine {
protected:
	Array<KPState *> states;
	KPState * currentState = nullptr;
	bool pendingState = false;

public:
	KPStateMachine() : states() {}

	int statesCount() {
		return states.size;
	}

	const KPState * getCurrentState() {
		return currentState;
	}

	const char * getCurrentStateName() {
    	return currentState->name;
	}

	const KPStateIdentifier getCurrentStateId() {
		return currentState->id;
	}

	KPState & getStateForIdentifier(KPStateIdentifier id) {
		for (int i = 0; i < states.size; i++) {
			KPState * state = states.get(i);
			if (state->id == id) {
				return *(state);
			}
		}

		while (true) {
			delay(2000);
			Serial.println("State Identifier has not been registered");
		}
	}

	KPState & getStateForIndex(int index) {
		return *(states.get(index));
	}

	bool isPendingState() {
		return pendingState;
	}

	bool isStateRegistered(KPStateIdentifier id) {
		for (int i = 0; i < states.size; i++) {
			KPState * state = states.get(i);
			if (state->id == id){
				return true;
			}
		}

		return false;
	}

	template<typename T = IdleState> KPState * registerState(KPStateIdentifier id, const char * name) {
		if (isStateRegistered(id)) {
			return nullptr;
		}

		KPState * state = new T();
		state->id = id;
		state->name = name;
		states.append(state);
		KPStatus::current().numberOfStates++;
		return state;
	}

	virtual void transitionTo(KPStateIdentifier id) {
		for (int i = 0; i < states.size; i++) {
			KPState * state = states.get(i);

			if (state->id == id) {
				pendingState = true;
				currentState = state;
				return;
			}
		}

		while (true) {
			Serial.print("State Unreachable: ");
			Serial.println(static_cast<int>(id));
			delay(2000);
		}
	}

	virtual void transitionTo(const char * name) {
		for (int i = 0; i < states.size; i++) {
			KPState * state = states.get(i);
			if (strcmp(state->name, name) == 0) {
				pendingState = true;
				currentState = state;
				return;
			}
		}

		while (true) {
			Serial.print("State Unreachable:");
			Serial.println(name);
			delay(2000);
		}
	}

	virtual void updateStateMachine() {
		if (currentState == nullptr) {
        	return;
		}

		if (pendingState) {
			currentState->begin();
			pendingState = false;
		}

		currentState->updateState(*this);
	}
};
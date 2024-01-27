#include <KPState.hpp>
#include <KPStateMachine.hpp>

void KPStateMachine::setup() {
	// do nothing
}

void KPStateMachine::update() {
	if (!currentState) {
		return;
	}

	if (!currentState->didEnter) {
		currentState->didEnter = true;
		currentState->enter(*this);
	}

	for (size_t i = 0; i < currentState->numberOfSchedules; i++) {
		auto & s = currentState->schedules[i];
		if (s.activated || !s.condition()) {
			continue;
		}

		s.activated = true;
		s.callback();
	}

	currentState->update(*this);
}

void KPStateMachine::next(int code) const {
	auto entry = mapNameToMiddleware.find(currentState->name);
	if (entry != mapNameToMiddleware.end()) {
		entry->second(code);
	}
}

void KPStateMachine::restart() {
	transitionTo(currentState ? currentState->name : nullptr);
}

void KPStateMachine::transitionTo(const char * name) {
	// Leave the current state
	if (currentState) {
		currentState->leave(*this);
	}

	// Move to new state
	auto next = mapNameToState[name];
	if (next) {
		println("Begin ", next->getName());
		currentState = next;
		currentState->begin();
		updateObservers(&KPStateMachineObserver::stateDidBegin, currentState);
	}
}

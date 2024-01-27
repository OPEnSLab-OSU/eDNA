#pragma once
#include <KPObserver.hpp>

class KPState;
class KPStateMachineObserver : public KPObserver {
public:
	const char * ObserverName() const {
		return KPStateMachineObserverName();
	}

	virtual const char * KPStateMachineObserverName() const {
		return "<Unnamed> State Machine Observer";
	}

	virtual void stateDidBegin(const KPState * newState) = 0;
};
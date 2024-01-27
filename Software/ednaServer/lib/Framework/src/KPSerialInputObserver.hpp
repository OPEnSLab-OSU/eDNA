#pragma once
#include <KPFoundation.hpp>
#include <KPObserver.hpp>

class KPSerialInputObserver : public KPObserver {
public:
	const char * ObserverName() const {
		return KPSerialInputObserverName();
	}

	virtual const char * KPSerialInputObserverName() const {
		return "<Unnamed> Serial Input Observer";
	}
	virtual void commandReceived(const char * line) = 0;
};
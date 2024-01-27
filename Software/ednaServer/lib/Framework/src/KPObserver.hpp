#pragma once

class KPObserver {
public:
	virtual const char * ObserverName() const {
		return "<Unnamed> Observer";
	}
};
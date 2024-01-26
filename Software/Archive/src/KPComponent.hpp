#pragma once
#include "Arduino.h"
#include "KPEventEmitter.hpp"

class KPComponent : public KPEventEmitter {
public:
    friend class KPSystem;
	const char * name;
    bool ready = true;
    
    KPComponent() : KPEventEmitter(), name("Unnamed Component") {}
    KPComponent(const char * name) : KPEventEmitter(), name(name) {}

protected:
    virtual void setup() {
    }
    
    virtual void update() {
    }

	virtual bool checkForConnection() {
		return true;
	}
};
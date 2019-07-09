#pragma once 
#include "Arduino.h"

class OPComponent {
public:
    friend class OPSystem;
	const char * name;
    bool ready = true;
	bool enable = true;
	// OPSystem * system = nullptr;
    
    OPComponent() {}
    OPComponent(const char * name) : name(name) {}

protected:
    virtual void setup() {
        // Serial.println("Init OPComponent");
    }
    
    virtual void update() {
        // Serial.println("Update OPComponent");
    }
};

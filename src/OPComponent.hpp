#pragma once 
#include "Arduino.h"

/*
    OPComponent defines two abstract methods that must be overrided by the subclass to
    provide injection onto the main loop.
    - setup()
    - loop()
*/
class OPComponent {
public:
    String name = "Unnamed";
    bool enabled = true;
    
    OPComponent() {}
    OPComponent(String name) :
    name(name) {
        
    }
    
    virtual void setup() {
        // Serial.println("Init OPComponent");
    }
    
    virtual void update() {
        // Serial.println("Update OPComponent");
    }
};

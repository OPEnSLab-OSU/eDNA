#pragma once
#include "OPComponent.hpp"

enum Direction {
    normal, reverse
};

class OPPump : public OPComponent {
public:
    using OPComponent::OPComponent;
    int control1, control2;

    OPPump(String name, int control1, int control2) : OPComponent(name) {
        this->control1 = control1;
        this->control2 = control2;
		pinMode(control1, OUTPUT);
		pinMode(control2, OUTPUT);
		off();
    }

    void on(Direction dir = normal) {
        digitalWrite(control1, dir == normal ? HIGH : LOW);
        digitalWrite(control2, dir == normal ? LOW : HIGH);
    }

    void off() {
        digitalWrite(control1, LOW);
        digitalWrite(control2, LOW);
    }
};
#pragma once
#include <KPFoundation.hpp>

enum class Direction {
    normal,
    reverse
};

class Pump : public KPComponent {
public:
    using KPComponent::KPComponent;
    int control1, control2;

    Pump(const char * name, int control1, int control2)
        : KPComponent(name) {
        this->control1 = control1;
        this->control2 = control2;
        pinMode(control1, OUTPUT);
        pinMode(control2, OUTPUT);
        off();
    }

    void on(Direction dir = Direction::normal) {
        delay(20);
        analogWrite(control1, dir == Direction::normal ? 255 : 0);
        analogWrite(control2, dir == Direction::normal ? 0 : 255);
    }

    void off() {
        analogWrite(control1, 0);
        analogWrite(control2, 0);
        delay(20);
    }

    void pwm(float _intensity, Direction dir = Direction::normal) {
        byte intensity = (byte)(constrain(_intensity, 0, 1) * 255);
        analogWrite(dir == Direction::normal ? control1 : control2, intensity);
        analogWrite(dir == Direction::normal ? control2 : control1, 0);
    }
};
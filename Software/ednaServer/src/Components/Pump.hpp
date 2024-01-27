#pragma once
#include <KPFoundation.hpp>
#include <Application/Constants.hpp>

class Pump : public KPComponent {
public:
    const int control1;
    const int control2;

    Pump(const char * name, int control1, int control2)
        : KPComponent(name),
          control1(control1),
          control2(control2) {
        pinMode(control1, OUTPUT);
        pinMode(control2, OUTPUT);
        off();
    }

    void on(Direction dir = Direction::normal) {
        digitalWrite(control1, dir == Direction::normal);
        digitalWrite(control2, dir != Direction::normal);
        delay(20);
    }

    void off() {
        digitalWrite(control1, 0);
        digitalWrite(control2, 0);
        delay(20);
    }

    void pwm(float duty_cycle, Direction dir = Direction::normal) {
        uint8_t intensity = constrain(duty_cycle, 0, 1) * 255;
        analogWrite(dir == Direction::normal ? control1 : control2, intensity);
        analogWrite(dir == Direction::normal ? control2 : control1, 0);
    }
};
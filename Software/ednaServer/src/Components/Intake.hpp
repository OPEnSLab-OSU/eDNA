#include <KPFoundation.hpp>
#include <Application/Constants.hpp>
#include <Components/ShiftRegister.hpp>

class Intake : public KPComponent {
public:
    using KPComponent::KPComponent;
    virtual void on()  = 0;
    virtual void off() = 0;
};

class LatchIntake : public Intake {
public:
    ShiftRegister & shift;
    int controlPin = 0, reversePin = 1;

    LatchIntake(ShiftRegister & shift) : Intake("latch-intake"), shift(shift) {}
    void on() {
        shift.setPin(controlPin, HIGH);
        shift.setPin(reversePin, LOW);
        shift.write();
        delay(80);
    };

    /** ────────────────────────────────────────────────────────────────────────────
     *  For a latch valve, this method does not actually close the valve. It sets the valve to open
     *  in the opposite direction.
     *
     *  ──────────────────────────────────────────────────────────────────────────── */
    void off() {
        shift.setPin(controlPin, LOW);
        shift.setPin(reversePin, HIGH);
        shift.write();
        delay(80);
    };
};

class BallIntake : public Intake {
public:
    ShiftRegister & shift;
    int controlPin = 0, reversePin = 1;

    BallIntake(ShiftRegister & shift) : Intake("ball-intake"), shift(shift) {}
    void on() {
        shift.setPin(controlPin, HIGH);
        shift.setPin(reversePin, LOW);
        shift.write();
    }

    void off() {
        shift.setPin(controlPin, LOW);
        shift.setPin(reversePin, HIGH);
        shift.write();
    }
};
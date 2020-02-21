#include <Components/Power.hpp>

volatile unsigned long rtcInterruptStart = 0;
volatile bool alarmTriggered             = true;

void isr() {
    if ((unsigned long) (millis() - rtcInterruptStart) < 1000) {
        return;
    }

    // detachInterrupt(digitalPinToInterrupt(RTC_Interrupt_Pin));
    alarmTriggered    = true;
    rtcInterruptStart = millis();
}
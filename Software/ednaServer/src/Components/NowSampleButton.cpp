#include "Components/NowSampleButton.hpp"

volatile unsigned long buttonInterruptStart = 0;
volatile bool buttonTriggered             = false;

void button_isr() {
    if ((unsigned long) (millis() - buttonInterruptStart) < 3000) {
        return;
    }

    buttonTriggered    = true;
    buttonInterruptStart = millis();
}
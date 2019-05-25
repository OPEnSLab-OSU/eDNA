#pragma once
#include "OPComponent.hpp"

class OPSerialBuffer : public OPComponent {
private:
    String input;
    String line;

    void (*callback)(String &s);
public:
    using OPComponent::OPComponent;

    String currentLine() { 
        return line;
    }

    void clear() { 
        line = "";
    }

    void didReceiveSerialCommand(void (*callback)(String &s)) {
        this->callback = callback;
    }

    void setup() override {
        // Serial.begin(9600);
        // delay(1000);
    }

    void update() override {
        if (Serial.available() <= 0) {
            return;
        }

        char inputChar = Serial.read();
        if (inputChar == '\n') {
            line = input + ' ';
            input = "";

            callback(line);
            clear();
            return;
        }

        // System characters
        if (inputChar < 32) {
            return;
        }

        input += inputChar;
        Serial.print(inputChar);
    }
};
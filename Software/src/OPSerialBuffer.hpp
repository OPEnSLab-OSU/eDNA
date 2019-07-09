#pragma once
#include "OPComponent.hpp"

using SerialBufferFunctionPointer = void (*)(String & s);

class OPSerialBuffer : public OPComponent {
private:
    String input;
    SerialBufferFunctionPointer callback = nullptr;

public:
    using OPComponent::OPComponent;

    void didReceiveSerialCommand(SerialBufferFunctionPointer callback) {
        this->callback = callback;
    }

    void update() override {
        while (Serial.available() > 0) {
            char inputChar = Serial.read();
            if (inputChar == '\n') {
                Serial.println();
                callback(input + ' ');
                input = "";
                return;
            }

            // Ignore System characters
            if (inputChar >= 32) {
                input += inputChar;
                Serial.print(inputChar);
            }
        }
    }
};
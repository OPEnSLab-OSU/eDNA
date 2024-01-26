#pragma once
#include "KPFoundation.hpp"
#include "KPComponent.hpp"

class KPSerialCommand : public KPComponent {
private:
    String input;
public:
    using KPComponent::KPComponent;

    void update() override {
		
        while (Serial.available() > 0) {
            char inputChar = Serial.read();
            if (inputChar == '\n') {
				notify<String>(Event::SCCommandEntered, &input);
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
#pragma once
#include <KPFoundation.hpp>
#include <KPSubject.hpp>
#include <KPSerialInputObserver.hpp>
#include <vector>

class KPSerialInput : public KPComponent, public KPSubject<KPSerialInputObserver> {
private:
	KPStringBuilder<255> input;

public:
	using KPComponent::KPComponent;

	void update() {
		while (Serial.available() > 0) {
			char inputChar = Serial.read();
			if (inputChar == '\n') {
				updateObservers(&KPSerialInputObserver::commandReceived, input);
				input.clear();
				return;
			}

			// Ignore System characters
			if (inputChar >= 32) {
				input.print(inputChar);
			}
		}
	}

	static KPSerialInput & sharedInstance() {
		static KPSerialInput serial("shared-serial-input");
		return serial;
	}
};
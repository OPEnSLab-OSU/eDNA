#pragma once
#include "KPFoundation.hpp"

class KPEventEmitter {	
public:
	template<typename T> void notify(const char * evt, T * arg) {
		KPNotificationCenter & nc = KPNotificationCenter::shared();
		for (int i = 0; i < nc.count(); i++) {
			KPEventPair & pair = nc.get(i);
			if (strcmp(pair.name, evt) == 0) {
				Serial.print("Event: ");
				Serial.println(evt);
				pair.callback(reinterpret_cast<void *>(arg));
			}
		}
	}
};
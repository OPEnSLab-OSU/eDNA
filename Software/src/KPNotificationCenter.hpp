#pragma once 
#include "Arduino.h"
#include "KPArray.hpp"

namespace Event {
	static constexpr const char * SCCommandEntered 	{"CommandEntered"};
	static constexpr const char * TMTaskLoaded 		{"TaskLoaded"};
	static constexpr const char * TMTaskSaving 		{"TaskSaving"};
	static constexpr const char * LGFinishedLogging {"FinishedLogging"};
};

struct KPEventPair {
	const char * name;
	void (* callback)(void * arg);
};

class KPNotificationCenter {
private:
	Array<KPEventPair> events;
public:

	KPEventPair & get(int index) {
		return events.get(index);
	}

	int count() {
		return events.size;
	}

	void observe(const char * event, void (* callback)(void * arg)) {
		KPEventPair pair;
		pair.name = event;
		pair.callback = callback;
		events.append(pair);
	}

	static KPNotificationCenter & shared() {
		static KPNotificationCenter nc;
		return nc;
	}
};


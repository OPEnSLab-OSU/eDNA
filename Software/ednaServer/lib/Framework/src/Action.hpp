#pragma once
#include <KPFoundation.hpp>
#include <functional>
#include <vector>

class TimedAction {
public:
	bool removable		  = false;
	long start			  = 0;
	long interval		  = 0;
	signed char repeatFor = 0;
	KPString name;

	std::function<void()> callback;

public:
	TimedAction() = default;
	TimedAction(const char * name) : name(name) {}

	void begin() {
		removable = false;
		start	  = millis();
	}

	long timeElapsed() const {
		return millis() - start;
	}

	bool isReady() const {
		return timeElapsed() >= interval;
	}
};

class ActionScheduler : public KPComponent {
public:
	std::vector<TimedAction> actions;

	ActionScheduler(const char * name) : KPComponent(name) {}

	void add(const TimedAction & action) {
		actions.push_back(action);
		actions.back().begin();
	}

	void markForRemoval(const char * name) {
		for (auto & action : actions) {
			if (action.name == name) {
				action.removable = true;
			}
		}
	}

	void removeCompletedActions() {
		auto predicate = std::remove_if(actions.begin(), actions.end(),
										[](const TimedAction & a) { return a.removable; });

		actions.erase(predicate, actions.end());
	}

	void update() override {
		removeCompletedActions();

		for (auto & action : actions) {
			if (action.isReady()) {
				action.callback();
			} else {
				continue;
			}

			if (action.repeatFor == 0) {
				action.removable = true;
			} else {
				action.begin();
				action.repeatFor = std::max(action.repeatFor - 1, -1);
			}
		}
	}

	static ActionScheduler & sharedInstance() {
		static ActionScheduler shared("Shared ActionScheduler");
		return shared;
	}
};

inline void run(const TimedAction & action) {
	ActionScheduler::sharedInstance().add(action);
}

inline void run(long delay, std::function<void()> callback) {
	TimedAction action;
	action.interval = delay;
	action.callback = callback;
	ActionScheduler::sharedInstance().add(action);
}

inline void runForever(long delay, const char * name, std::function<void()> callback) {
	TimedAction action;
	action.name		 = name;
	action.interval	 = delay;
	action.callback	 = callback;
	action.repeatFor = -1;
	ActionScheduler::sharedInstance().add(action);
}

inline void cancel(const char * name) {
	ActionScheduler::sharedInstance().markForRemoval(name);
}

#pragma once
#include <KPFoundation.hpp>
#include <KPStateMachine.hpp>
#include <functional>
#include <vector>

struct KPStateSchedule {
private:
	bool activated = false;
	friend class KPStateMachine;

public:
	std::function<bool()> condition;
	std::function<void()> callback;

	// KPStateSchedule(long time, std::function<void()> callback) : time(time), callback(callback)
	// {}

	KPStateSchedule(std::function<bool()> condition, std::function<void()> callback)
		: condition(condition),
		  callback(callback) {}
};

class KPState {
	friend class KPStateMachine;

protected:
	const char * name		= nullptr;
	unsigned long startTime = 0;
	bool didEnter			= false;
	size_t numberOfSchedules	= 0;
	std::vector<KPStateSchedule> schedules;

	void begin() {
		startTime		= millis();
		numberOfSchedules = 0;
		didEnter		= false;
	}

	void reserve(size_t size) {
		schedules.reserve(size);
	}

public:
	const char * getName() const {
		return name;
	}

	/** ────────────────────────────────────────────────────────────────────────────
	 *  @brief REQUIRED Subclass must override this method and specify the behavior
	 * when entering this state
	 *
	 *  @param machine
	 *  ──────────────────────────────────────────────────────────────────────────── */
	virtual void enter(KPStateMachine & machine) = 0;

	/** ────────────────────────────────────────────────────────────────────────────
	 *  @brief When override, this method is called just before the transition to new
	 *  state
	 *
	 *  @param machine State machine owning this state
	 *  ──────────────────────────────────────────────────────────────────────────── */
	virtual void leave(KPStateMachine & machine) {}

	/** ────────────────────────────────────────────────────────────────────────────
	 *  @brief Override to receive runtime lifecycle loop while in this state
	 *
	 *  @param machine State machine owning this state
	 *  ──────────────────────────────────────────────────────────────────────────── */
	virtual void update(KPStateMachine & machine) {}

	// TODO: Maybe a good idea
	// virtual void setValuesFromJson(const JsonVariant & data) {}

	unsigned long timeSinceLastTransition() const {
		return millis() - startTime;
	}

	void setTimeCondition(unsigned long seconds, std::function<void()> callback) {
		unsigned long millis = secsToMillis(seconds);
		setCondition([this, millis]() { return timeSinceLastTransition() >= millis; }, callback);
	}

	void setCondition(std::function<bool()> condition, std::function<void()> callback) {
		if (numberOfSchedules == schedules.size()) {
			schedules.push_back(KPStateSchedule(condition, callback));
		} else {
			schedules[numberOfSchedules] = KPStateSchedule(condition, callback);
		}

		numberOfSchedules++;
	}
};
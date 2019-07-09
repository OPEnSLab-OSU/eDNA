#pragma once
#include "Definitions.hpp"
#include "OPComponent.hpp"
#include "Status.hpp"
#include "LowPower.h"
#include "DS3232RTC.h"
#include "Wire.h"

#define RTC_ADDR 0x68 		// Library already has the address but this is needed for checking
#define RTC_INTERRUPT_PIN 3
#define POWER_MODULE_PIN A0

static unsigned long rtcInterruptStart = 0;
static bool alarmTriggered = false;

static void wakeup() {
    if ((unsigned long) (millis() - rtcInterruptStart) < 500) {
        return;
    }
	
    detachInterrupt(digitalPinToInterrupt(RTC_INTERRUPT_PIN));
    alarmTriggered = true;
    rtcInterruptStart = millis();
}

class OPPower : public OPComponent {
	public:
		static DS3232RTC rtc;
	public:
	using OPComponent::OPComponent;

	void setup() override;
	void update() override;

	void waitForTimeKeeperConnection();

    // Set alarms registers to a known value and clear any prev alarms
    void resetAlarms();
	void disarmAlarms();

    // Bring the chip to the low power mode. External Interrupt requires to return to regular operation
    void sleepForever();
    void sleepFor(int seconds);

	void scheduleNextAlarm(Status s);
	void scheduleNextAlarm(time_t interval);

	// Serial print the current time
    void printCurrentTime();
	void printTime(TimeElements & tm);
	void printTime(time_t seconds);
	time_t compileTime();
};


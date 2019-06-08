#pragma once
#include "Foundation.hpp"
#include "Status.hpp"
#include "LowPower.h"
#include "DS3232RTC.h"
#include "Wire.h"

#define RTC_ADDR 0x68 		// Library already has the address but this is needed for checking
#define RTC_INTERRUPT_PIN 3
#define POWER_MODULE_PIN A0

unsigned long rtcInterruptStart = 0;
bool alarmTriggered = false;

DS3232RTC rtc(false);

void wakeup() {
    if ((unsigned long) (millis() - rtcInterruptStart) < 500) {
        return;
    }
	
    detachInterrupt(digitalPinToInterrupt(RTC_INTERRUPT_PIN));
    alarmTriggered = true;
    rtcInterruptStart = millis();
}

class OPPower : public OPComponent {
	public:
	using OPComponent::OPComponent;

	void waitForTimeKeeperConnection() {
		while (true) {
            Wire.beginTransmission(RTC_ADDR);
            Wire.write(1);
            Wire.endTransmission();
            Wire.requestFrom(RTC_ADDR, 1);

			// This means that all bits are high. I2C for DS3231 is active low.
            if (Wire.read() != -1) {
                Serial.println(F("\n-= RTC Connected =-"));
                break;
            } else {
                Serial.println(F("\n-= RTC Not Connected =-"));
                delay(2000);
            }
        }
	}

	void setup() override {
        // Check if RTC is connected
        waitForTimeKeeperConnection();
		
		// Initilize RTC I2C Bus
		rtc.begin();

        // Reset RTC to a known state, clearing alarms, 
        rtc.setAlarm(ALM1_MATCH_DATE, 0, 0, 0, 1);
		rtc.setAlarm(ALM2_MATCH_DATE, 0, 0, 0, 1);
		rtc.alarm(ALARM_1);
		rtc.alarm(ALARM_2);
		rtc.alarmInterrupt(ALARM_1, false);
		rtc.alarmInterrupt(ALARM_2, false);
		rtc.squareWave(SQWAVE_NONE);

		// FOR M0 Only: 
		// Set the RTC time and date to the compile time when connected to computer
		if (Serial) {
			Serial.print("Setting compile time: ");
			Serial.println(rtc.set(compileTime()));
		}

		// Print out the time
        printCurrentTime();
	}

	void update() override {
		if (rtc.alarm(ALARM_1) || rtc.alarm(ALARM_2)) {
			alarmTriggered = true;
		}
	}

    //------------------------------------------------------------------------------------
    // Set alarms registers to a known value and clear any prev alarms
    //------------------------------------------------------------------------------------
    void resetAlarms() {
		rtc.setAlarm(ALM1_MATCH_DATE, 0, 0, 0, 1);
		rtc.setAlarm(ALM2_MATCH_DATE, 0, 0, 0, 1);
		disarmAlarms();
    }

	void disarmAlarms() {
		rtc.alarm(ALARM_1);
		rtc.alarm(ALARM_2);
	}

    //------------------------------------------------------------------------------------
    // Serial print the current time
    //------------------------------------------------------------------------------------
    void printCurrentTime() {
		char message[32] = {0};
		tmElements_t tm;
		rtc.read(tm);
		sprintf(message, "%s %uH %uM %uS", dayShortStr(tm.Day-1), tm.Hour, tm.Minute, tm.Second);
		
		Serial.print(F("Current Time: "));
		Serial.println(message);
    }

    //------------------------------------------------------------------------------------
    // Bring the chip to the low power mode. External Interrupt requires to return to regular operation
    //------------------------------------------------------------------------------------
    void sleepForever() {
        Serial.println();
        Serial.println(F("Going to sleep..."));
        for (int i = 3; i > 0; i--){
            Serial.print(F("-> "));
            Serial.println(i);
            delay(333);
        }

        attachInterrupt(digitalPinToInterrupt(RTC_INTERRUPT_PIN), wakeup, FALLING);		
		LowPower.standby();

        Serial.println();
        Serial.println(F("Just woke up!"));
        printCurrentTime();
    }

    void sleepFor(int seconds) {
		time_t now = rtc.get();
		tmElements_t future;
		breakTime(now + seconds, future);

        detachInterrupt(digitalPinToInterrupt(RTC_INTERRUPT_PIN));
        disarmAlarms();
        rtc.setAlarm(ALM1_MATCH_HOURS, future.Second, future.Minute, future.Hour, 0);
        rtc.alarmInterrupt(1, true);
        sleepForever();
    }

	void scheduleNextAlarm(Status s) {
		time_t now = rtc.get();
		// tmElements_t now;
	}

	void scheduleNextAlarm(time_t interval) {
		TimeElements future;
		breakTime(rtc.get() + interval, future);
		rtc.setAlarm(ALM1_MATCH_MINUTES, future.Second, future.Minute, future.Hour, 0);
	}

	time_t compileTime() {
		const time_t FUDGE(10);			//Fudge factor to allow for upload time
		const char *compDate = __DATE__;
		const char *compTime = __TIME__;
		const char *months = "JanFebMarAprMayJunJulAugSepOctNovDec";
		char *m;

		char compMon[4];
		compMon[3] = '\0';

		strncpy(compMon, compDate, 3); 	// Get month from compiled date
		m = strstr(months, compMon);

		tmElements_t tm;
		tm.Month = ((m - months) / 3 + 1);
		tm.Day = atoi(compDate + 4);
		tm.Year = atoi(compDate + 7) - 1970;
		tm.Hour = atoi(compTime);
		tm.Minute = atoi(compTime + 3);
		tm.Second = atoi(compTime + 6);

		time_t t = makeTime(tm);
		return t + FUDGE;        		//Add fudge factor to allow for compile time
	}
};


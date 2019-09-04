#pragma once
#include "DS3232RTC.h"
#include "KPComponent.hpp"
#include "KPFoundation.hpp"
#include "LowPower.h"
#include "Wire.h"

#define RTC_ADDR 0x68  // Library already has the address but this is needed for checking

//===========================================================
// [+_+] 
//===========================================================

static ulong rtcInterruptStart = 0;
static bool alarmTriggered = false;

static void isr() {
    if ((ulong)(millis() - rtcInterruptStart) < 300) {
        return;
    }

    detachInterrupt(digitalPinToInterrupt(RTC_Interrupt_Pin));
    alarmTriggered = true;
    rtcInterruptStart = millis();
}

class KPPowerManagement : public KPComponent {
public:
    DS3232RTC timeKeeper;

    KPPowerManagement() : KPComponent(), timeKeeper(false) {
		pinMode(Power_Module_Pin, OUTPUT);
    }

    void setup() override {
        // Check if RTC is connected
        waitForConnection();

        // Initilize RTC I2C Bus
        timeKeeper.begin();

        // Reset RTC to a known state, clearing alarms, clear interrupts
        resetAlarms();
        timeKeeper.squareWave(SQWAVE_NONE);

        // Register RTC as the external time provider for Time library
        setSyncProvider(timeKeeper.get);
        setTime(timeKeeper.get());

        // Print out the current time
        Serial.println("Initial Startup: ");
        printCurrentTime();

		// Register interrupt pin as active low
		pinMode(RTC_Interrupt_Pin, INPUT_PULLUP);
    }

    void update() override {
        // if (timeKeeper.alarm(ALARM_1) || timeKeeper.alarm(ALARM_2)) {
        //     alarmTriggered = true;
        // }
    }

	//===========================================================
	// [+_+] Wait for RTC connection 
	// This is done by asking the RTC to return 
	//===========================================================
    void waitForConnection() {
        while (true) {
            Wire.begin();
            // Wire.beginTransmission(RTC_ADDR);
            // Wire.write(1);
            // Wire.endTransmission();
            Wire.requestFrom(RTC_ADDR, 1);

            // This means that all bits are high. I2C for DS3231 is active low.
            if (Wire.read() != -1) {
                Serial.println(F("\n-= RTC Connected =-"));
                ready = true;
                break;
            } else {
                Serial.println(F("\n-= RTC Not Connected =-"));
                ready = false;
                delay(2000);
            }
        }
    }

	void shutdown() {
		digitalWrite(Power_Module_Pin, HIGH);
		delay(20);
		digitalWrite(Power_Module_Pin, LOW);
	}
	
	//===========================================================
	// [+_+] Set alarms registers to a known value and clear any prev alarms
	//===========================================================
    void resetAlarms() {
        timeKeeper.setAlarm(ALM1_MATCH_DATE, 0, 0, 0, 1);
        timeKeeper.setAlarm(ALM2_MATCH_DATE, 0, 0, 0, 1);
        disarmAlarms();
    }

	//===========================================================
	// [+_+] Clear previous alarms and disable interrupts
	//===========================================================
    void disarmAlarms() {
        timeKeeper.alarm(ALARM_1);
        timeKeeper.alarm(ALARM_2);
        timeKeeper.alarmInterrupt(ALARM_1, false);
        timeKeeper.alarmInterrupt(ALARM_2, false);
    }

	//===========================================================
	// [+_+] Bring the chip to the low power mode.
	// External interrupt is required to awake the device and resume operation
	//===========================================================
    void sleepForever() {
        Serial.println();
        Serial.println(F("Going to sleep..."));
        for (int i = 3; i > 0; i--) {
            Serial.print(F("-> "));
            Serial.println(i);
            delay(333);
        }

        LowPower.standby();
        Serial.println();
        Serial.println(F("Just woke up due to interrupt!"));
        printCurrentTime();
    }

	//===========================================================
	// [+_+] Put the chip into the low power state for specified number of seconds
	//===========================================================
    void sleepFor(ulong seconds) {
        setTimeout(seconds, true);
        sleepForever();
    }

	//===========================================================
	// [+_+] Schedule alarm for specified number of seconds from now.
	// @param bool usingInterrupt: A flag controlling whether to trigger the interrupt service routine
	//===========================================================
    void setTimeout(ulong seconds, bool usingInterrupt) {
        TimeElements future;
        breakTime(timeKeeper.get() + seconds, future);
        disarmAlarms();
        timeKeeper.setAlarm(ALM1_MATCH_MINUTES, future.Second, future.Minute, future.Hour, 0);
        if (usingInterrupt) {
            attachInterrupt(digitalPinToInterrupt(RTC_Interrupt_Pin), isr, FALLING);
            timeKeeper.alarmInterrupt(1, true);
        }
    }

	//===========================================================
	// [+_+] Schedule RTC alarm given TimeElements
	// @param TimeElements future: must be in the future, otherwise this method does nothing
	//===========================================================
    void scheduleNextAlarm(TimeElements future) {
        ulong seconds = makeTime(future);
        ulong timestamp = now();
        if (seconds < timestamp) {
            return;
        }

        setTimeout(seconds - timestamp, true);
    }

	//===========================================================
	// [+_+] Set RTC time and internal timer of the Time library
	//===========================================================
    void set(ulong seconds) {
        Serial.print("Setting RTC Time...");
        printTime(seconds);

        setTime(seconds);
        timeKeeper.set(seconds);
    }

	//===========================================================
	// [+_+] Print time to console formatted as YYYY.MM.DD : hh.mm.ss
	//===========================================================
    void printTime(ulong seconds) {
        char message[64]{};
        sprintf(message, "%u.%u.%u : %u.%u.%u", year(seconds), month(seconds),
                day(seconds), hour(seconds), minute(seconds), second(seconds));
        Serial.println(message);
    }

    void printTime(ulong seconds, int offset) {
        printTime(seconds + (offset * 60 * 60));
    }

    void printCurrentTime(int offset = 0) {
        Serial.print("Current Time: ");
        printTime(timeKeeper.get(), offset);
    }

	//===========================================================
	// [+_+] Convert compiled timestrings to seconds since 1 Jan 1970
	//===========================================================
    time_t compileTime() {
        const time_t FUDGE = 10;  //Fudge factor to allow for upload time
        const char * compDate = __DATE__;
        const char * compTime = __TIME__;
        const char * months = "JanFebMarAprMayJunJulAugSepOctNovDec";
        char * m;

        char compMon[4];
        compMon[3] = 0;  // NULL character

        strncpy(compMon, compDate, 3);  // Get month from compiled date
        m = strstr(months, compMon);

        tmElements_t tm;
        tm.Month = ((m - months) / 3 + 1);
        tm.Day = atoi(compDate + 4);
        tm.Year = atoi(compDate + 7) - 1970;
        tm.Hour = atoi(compTime);
        tm.Minute = atoi(compTime + 3);
        tm.Second = atoi(compTime + 6);

        time_t t = makeTime(tm);
        Serial.println(compDate);
        Serial.println(compTime);
        return t + FUDGE;  //Add fudge factor to allow for compile time
    }
};

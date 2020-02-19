#pragma once
#include <KPFoundation.hpp>
#include <Application/Constants.hpp>
#include <DS3232RTC.h>
#include <LowPower.h>
#include <Wire.h>

#define RTC_ADDR 0x68  // Library already has the address but this is needed for checking

//===========================================================
// [+_+]
//===========================================================
volatile extern unsigned long rtcInterruptStart;
volatile extern bool alarmTriggered;
extern void isr();

class Power : public KPComponent {
public:
    DS3232RTC rtc;

    Power(const char * name)
        : KPComponent(name), rtc(false) {
        pinMode(Power_Module_Pin, OUTPUT);
    }

    void setup() override {
        // Check if RTC is connected
        waitForConnection();

        // Initilize RTC I2C Bus
        rtc.begin();

        // Reset RTC to a known state, clearing alarms, clear interrupts
        resetAlarms();
        rtc.squareWave(SQWAVE_NONE);

        // Register RTC as the external time provider for Time library
        setSyncProvider(rtc.get);
        setTime(rtc.get());

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
                println(F("\n-= RTC Connected =-"));
                // enabled = true;
                break;
            } else {
                println(F("\n-= RTC Not Connected =-"));
                // enabled = false;
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
        rtc.setAlarm(ALM1_MATCH_DATE, 0, 0, 0, 1);
        rtc.setAlarm(ALM2_MATCH_DATE, 0, 0, 0, 1);
        disarmAlarms();
    }

    //===========================================================
    // [+_+] Clear previous alarms and disable interrupts
    //===========================================================
    void disarmAlarms() {
        rtc.alarm(ALARM_1);
        rtc.alarm(ALARM_2);
        rtc.alarmInterrupt(ALARM_1, false);
        rtc.alarmInterrupt(ALARM_2, false);
    }

    //===========================================================
    // [+_+] Bring the chip to the low power mode.
    // External interrupt is required to awake the device and resume operation
    //===========================================================
    void sleepForever() {
        println();
        println(F("Going to sleep..."));
        for (int i = 3; i > 0; i--) {
            Serial.print(F("-> "));
            Serial.println(i);
            delay(333);
        }

        LowPower.standby();
        println();
        println(F("Just woke up due to interrupt!"));
        printCurrentTime();
    }

    //===========================================================
    // [+_+] Put the chip into the low power state for specified number of seconds
    //===========================================================
    void sleepFor(unsigned long seconds) {
        setTimeout(seconds, true);
        sleepForever();
    }

    //===========================================================
    // [+_+] Schedule alarm for specified number of seconds from now.
    // @param bool usingInterrupt: A flag controlling whether to trigger the interrupt service routine
    //===========================================================
    void setTimeout(time_t seconds, bool usingInterrupt) {
        TimeElements future;
        breakTime(rtc.get() + seconds, future);
        disarmAlarms();
        rtc.setAlarm(ALM1_MATCH_MINUTES, future.Second, future.Minute, future.Hour, 0);
        if (usingInterrupt) {
            attachInterrupt(digitalPinToInterrupt(RTC_Interrupt_Pin), isr, FALLING);
            rtc.alarmInterrupt(1, true);
        }
    }

    //===========================================================
    // [+_+] Schedule RTC alarm given TimeElements
    // @param TimeElements future: must be in the future, otherwise this method does nothing
    //===========================================================
    void scheduleNextAlarm(TimeElements future) {
        unsigned long seconds   = makeTime(future);
        unsigned long timestamp = now();
        if (seconds < timestamp) {
            return;
        }

        setTimeout(seconds - timestamp, true);
        println("Alarm triggering in: ", seconds - timestamp);
    }

    void scheduleNextAlarm(time_t utc) {
        time_t timestamp = now();
        if (utc >= timestamp) {
            println(utc - timestamp);
            setTimeout(utc - timestamp, true);
        }
    }

    //===========================================================
    // [+_+] Set RTC time and internal timer of the Time library
    //===========================================================
    void set(time_t seconds) {
        print("Setting RTC Time...");
        printTime(seconds);

        setTime(seconds);
        rtc.set(seconds);
    }

    //===========================================================
    // [+_+] Print time to console formatted as YYYY.MM.DD : hh.mm.ss
    //===========================================================
    void printTime(time_t seconds) {
        const char * format = "%d.%d.%d : %d.%d.%d";
        Serial.printf(format, year(seconds), month(seconds), day(seconds),
                      hour(seconds), minute(seconds), second(seconds));
    }

    void printTime(time_t seconds, int offset_hours) {
        printTime(seconds + (offset_hours * 60 * 60));
    }

    void printCurrentTime(int offset_hours = 0) {
        print("Current Time: ");
        printTime(now(), offset_hours);
    }

    //===========================================================
    // [+_+] Convert compiled timestrings to seconds since 1 Jan 1970
    //===========================================================
    time_t compiledTime() {
        const time_t FUDGE    = 10;  //Fudge factor to allow for upload time
        const char * compDate = __DATE__;
        const char * compTime = __TIME__;
        const char * months   = "JanFebMarAprMayJunJulAugSepOctNovDec";

        char compMon[4]{0};
        strncpy(compMon, compDate, 3);  // Get month from compiled date

        char * m = strstr(months, compMon);

        TimeElements tm;
        tm.Month  = ((m - months) / 3 + 1);
        tm.Day    = atoi(compDate + 4);
        tm.Year   = atoi(compDate + 7) - 1970;
        tm.Hour   = atoi(compTime);
        tm.Minute = atoi(compTime + 3);
        tm.Second = atoi(compTime + 6);

        time_t t = makeTime(tm);
		print("Compiled time: ");
        printCurrentTime(t);
        return t + FUDGE;  //Add fudge factor to allow for upload time
    }
};

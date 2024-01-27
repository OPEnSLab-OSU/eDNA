/**
 * Copyright (c) 2020 Kawin Pechetratanapanit
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once
#include <KPFoundation.hpp>
#include <DS3232RTC.h>
#include <LowPower.h>
#include <Wire.h>
#include <SPI.h>
#include <functional>

#include <Application/Constants.hpp>

#define RTC_ADDR 0x68

//
// ──────────────────────────────────────────────────────────────────────── I ──────────
//   :::::: P O W E R   M A N A G E M E N T : :  :   :    :     :        :          :
// ──────────────────────────────────────────────────────────────────────────────────
//
// DS3232RTC Library already know the address internally but we define it again here the
// to check for connection
//

extern volatile unsigned long rtcInterruptStart;
extern volatile bool alarmTriggered;
extern void rtc_isr();

class Power : public KPComponent {
public:
    DS3232RTC rtc;
    std::function<void()> interruptCallback;

    Power(const char * name) : KPComponent(name), rtc(false) {}

    void onInterrupt(std::function<void()> callbcak) {
        interruptCallback = callbcak;
    }

    void setupRTC() {
        // Initilize RTC I2C Bus
        waitForConnection();
        rtc.begin();

        // Reset RTC to a known state, clearing alarms, clear interrupts
        resetAlarms();
        rtc.squareWave(SQWAVE_NONE);

        // Register RTC as the external time provider for Time library
        setSyncProvider(rtc.get);
        setTime(rtc.get());

        // Print out the current time
        print("RTC startup time: ");
        printCurrentTime();
        println();
    }

    void setup() override {
        setupRTC();

        // Register interrupt pin as active low
        // When in programming mode, the RTC will be disconnected from the external pullup resistor,
        // so we need this internal one. INPUT_PULLUP is required.
        pinMode(HardwarePins::RTC_INTERRUPT, INPUT_PULLUP);
        pinMode(HardwarePins::POWER_MODULE, OUTPUT);
    }

    /** ────────────────────────────────────────────────────────────────────────────
     *  Runtime update loop. Check if RTC has been triggered.
     *  ──────────────────────────────────────────────────────────────────────────── */
    void update() override {
        if (!alarmTriggered || !interruptCallback) {
            return;
        }

        // Check if the interrupt is comming from RTC
        // This is important in noisy environment
        if (rtc.alarm(1) || rtc.alarm(2)) {
            disarmAlarms();
            noInterrupts();
            interruptCallback();
            alarmTriggered = false;
        }
    }

    /** ────────────────────────────────────────────────────────────────────────────
     *  Wait for RTC to connect.
     *
     *  This is done by checking if the return byte from the RTC is -1. In two
     *  complement's signed interger, all bits high means -1.Since, I2C for DS3231 is
     *  active low, this means that the RTC is not connected
     *  ──────────────────────────────────────────────────────────────────────────── */
    void waitForConnection() {
        Wire.begin();
        for (;; delay(5000)) {
            Wire.requestFrom(RTC_ADDR, 1, false);  // false: don't release I2C line
            if (Wire.read() == -1) {
                println(RED("RTC not connected"));
            } else {
                println(GREEN("RTC connected"));
                break;
            }
        }

        Wire.end();
    }

    /** ────────────────────────────────────────────────────────────────────────────
     *  Signal the power module to cut power from the system
     *
     *  ──────────────────────────────────────────────────────────────────────────── */
    void shutdown() {
        digitalWrite(HardwarePins::POWER_MODULE, HIGH);
        delay(20);
        digitalWrite(HardwarePins::POWER_MODULE, LOW);
    }

    /** ────────────────────────────────────────────────────────────────────────────
     *  Set alarms registers to a known value and clear any prev alarms
     *
     *  ──────────────────────────────────────────────────────────────────────────── */
    void resetAlarms() {
        rtc.setAlarm(ALM1_MATCH_DATE, 0, 0, 0, 1);
        rtc.setAlarm(ALM2_MATCH_DATE, 0, 0, 0, 1);
        disarmAlarms();
    }

    /** ────────────────────────────────────────────────────────────────────────────
     *  Clear previous alarms and disable interrupts
     *
     *  ──────────────────────────────────────────────────────────────────────────── */
    void disarmAlarms() {
        rtc.alarm(ALARM_1);
        rtc.alarm(ALARM_2);
        rtc.alarmInterrupt(ALARM_1, false);
        rtc.alarmInterrupt(ALARM_2, false);
    }

    /** ────────────────────────────────────────────────────────────────────────────
     *  Bring the chip to the low power mode.
     *
     *  External interrupt is required to wake the device and resume operation
     *  ──────────────────────────────────────────────────────────────────────────── */
    void sleepForever() {
        println();
        println("Going to sleep...");
        for (int i = 3; i > 0; i--) {
            print(F("-> "));
            println(i);
            delay(333);
        }

        LowPower.standby();
        println();
        println("Just woke up due to interrupt!");
        printCurrentTime();
    }

    /** ────────────────────────────────────────────────────────────────────────────
     *  Put the chip into the low power state for specified number of seconds
     *
     *  @param seconds How long in seconds
     *  ──────────────────────────────────────────────────────────────────────────── */
    void sleepFor(unsigned long seconds) {
        setTimeout(seconds, true);
        sleepForever();
    }

    /** ────────────────────────────────────────────────────────────────────────────
     *  Schedule alarm for specified number of seconds from now
     *
     *  @param seconds How long until alarm
     *  @param usingInterrupt If true, the rtc fires interrupt at HardwarePins::RTC_INTERRUPT
     *  ──────────────────────────────────────────────────────────────────────────── */
    void setTimeout(unsigned long seconds, bool usingInterrupt) {
        TimeElements future;
        breakTime(rtc.get() + seconds, future);
        disarmAlarms();
        rtc.setAlarm(ALM1_MATCH_MINUTES, future.Second, future.Minute, future.Hour, 0);
        if (usingInterrupt) {
            attachInterrupt(digitalPinToInterrupt(HardwarePins::RTC_INTERRUPT), rtc_isr, FALLING);
            rtc.alarmInterrupt(1, true);
        }
    }

    /** ────────────────────────────────────────────────────────────────────────────
     *  Schedule RTC alarm given TimeElements
     *
     *  @param future Must be in the future, otherwise this method does nothing
     *  ──────────────────────────────────────────────────────────────────────────── */
    void scheduleNextAlarm(TimeElements future) {
        unsigned long utc = makeTime(future);
        scheduleNextAlarm(utc);
    }

    /** ────────────────────────────────────────────────────────────────────────────
     *  Schedule RTC alarm given utc
     *
     *  @param utc Must be in the future, otherwise this method does nothing
     *  ──────────────────────────────────────────────────────────────────────────── */
    void scheduleNextAlarm(unsigned long utc) {
        unsigned long timestamp = now();
        if (utc < timestamp) {
            return;
        }

        println("Alarm triggering in: ", utc - timestamp, " seconds");
        setTimeout(utc - timestamp, true);
    }

    /** ────────────────────────────────────────────────────────────────────────────
     *  Set RTC time and internal timer of the Time library
     *
     *  @param seconds
     *  ──────────────────────────────────────────────────────────────────────────── */
    void set(unsigned long seconds) {
        println("Setting RTC Time...");
        printTime(seconds);

        setTime(seconds);  // Set time in Time library
        rtc.set(seconds);  // Set time for RTC
    }

    /** ────────────────────────────────────────────────────────────────────────────
     *  Print time to console formatted as YYYY/MM/DD hh:mm:ss GMT +/- offset
     *
     *  @param utc Unix epoch
     *  @param offset Offset in time zone
     *  ──────────────────────────────────────────────────────────────────────────── */
    void printTime(unsigned long utc, int offset = 0) {
        char message[64];
        utc = utc + (offset * 60 * 60);
        sprintf(message, "%u/%u/%u %02u:%02u:%02u GMT %+d", year(utc), month(utc), day(utc),
                hour(utc), minute(utc), second(utc), offset);
        print(message);
    }

    void printCurrentTime(int offset = 0) {
        printTime(rtc.get(), offset);
    }

    /** ────────────────────────────────────────────────────────────────────────────
     *  Convert compiled timestrings to seconds since 1 Jan 1970
     *
     *  @param offsetMinutes Minutes to offset time b (default: 0)
     *  @return time_t Time in seconds since 1 Jan 1970
     *  ──────────────────────────────────────────────────────────────────────────── */
    time_t compileTime(int offsetMinutes = 0) {
        const char * date   = __DATE__;
        const char * time   = __TIME__;
        const char * months = "JanFebMarAprMayJunJulAugSepOctNovDec";
        char * m;

        // Get month from compiled date
        char month[4]{0};
        strncpy(month, date, 3);
        m = strstr(months, month);

        TimeElements tm;
        tm.Month  = ((m - months) / 3 + 1);
        tm.Day    = atoi(date + 4);
        tm.Year   = atoi(date + 7) - 1970;
        tm.Hour   = atoi(time);
        tm.Minute = atoi(time + 3);
        tm.Second = atoi(time + 6);

        time_t t = makeTime(tm);
        if (offsetMinutes) {
            t += offsetMinutes * 60;
            breakTime(t, tm);
        }

        // Add FUDGE factor to allow for time to compile
        constexpr time_t FUDGE = 10;
        return t + FUDGE;
    }
};
#include "OPPower.hpp"

DS3232RTC OPPower::rtc(false);

void OPPower::waitForTimeKeeperConnection() {
    while (true) {
        Wire.beginTransmission(RTC_ADDR);
        Wire.write(1);
        Wire.endTransmission();
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

void OPPower::setup() {
    // Check if RTC is connected
	setTime(compileTime());
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

	setSyncProvider(rtc.get);

    // Print out the time
	Serial.println("Initial Startup: ");
    printCurrentTime();
}

void OPPower::update() {
    if (rtc.alarm(ALARM_1) || rtc.alarm(ALARM_2)) {
        alarmTriggered = true;
    }
}

void OPPower::resetAlarms() {
    rtc.setAlarm(ALM1_MATCH_DATE, 0, 0, 0, 1);
    rtc.setAlarm(ALM2_MATCH_DATE, 0, 0, 0, 1);
    disarmAlarms();
}

void OPPower::disarmAlarms() {
    rtc.alarm(ALARM_1);
    rtc.alarm(ALARM_2);
}

void OPPower::printCurrentTime() {
    tmElements_t tm;
    rtc.read(tm);
	printTime(tm);
}

void OPPower::printTime(TimeElements & tm) {
	char message[32] = {0};
	sprintf(message, "%uY %uM %s %uH %uM %uS", tmYearToY2k(tm.Year), tm.Month, dayShortStr(tm.Wday), tm.Hour, tm.Minute, tm.Second);
	Serial.print(F("Current Time: "));
    Serial.println(message);
}

void OPPower::printTime(time_t seconds) {
	TimeElements tm;
	breakTime(seconds, tm);
	printTime(tm);
}

void OPPower::sleepForever() {
    Serial.println();
    Serial.println(F("Going to sleep..."));
    for (int i = 3; i > 0; i--) {
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

void OPPower::sleepFor(int seconds) {
    time_t now = rtc.get();
    tmElements_t future;
    breakTime(now + seconds, future);

    detachInterrupt(digitalPinToInterrupt(RTC_INTERRUPT_PIN));
    disarmAlarms();
    rtc.setAlarm(ALM1_MATCH_HOURS, future.Second, future.Minute, future.Hour, 0);
    rtc.alarmInterrupt(1, true);
    sleepForever();
}

void OPPower::scheduleNextAlarm(Status s) {
    time_t now = rtc.get();
    // tmElements_t now;
}

void OPPower::scheduleNextAlarm(time_t interval) {
    TimeElements future;
    breakTime(rtc.get() + interval, future);
    rtc.setAlarm(ALM1_MATCH_MINUTES, future.Second, future.Minute, future.Hour, 0);
}

time_t OPPower::compileTime() {
    const time_t FUDGE(10);  		//Fudge factor to allow for upload time
    const char * compDate = __DATE__;
    const char * compTime = __TIME__;
    const char * months = "JanFebMarAprMayJunJulAugSepOctNovDec";
    char * m;

    char compMon[4];
    compMon[3] = 0; 				// NULL character

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
    return t + FUDGE;  				//Add fudge factor to allow for compile time
}
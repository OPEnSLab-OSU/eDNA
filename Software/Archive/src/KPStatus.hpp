#pragma once
#include "Arduino.h"

//===============================================================
// [+_+] Sington object describing POD for interfacing between components
//===============================================================
struct KPStatus {
	static const int valveCount	{24};
    int valveBegin 				{11};
    int valveLowerBound 		{9};
    int valveUpperBound 		{valveCount - 1};
	int valveCurrent 	 		{valveBegin};
	int isFull					{false};
	int valves[valveCount]		{0};

    int samplingInteval			{15};

    float pressure				{0};
    float temperature			{0};
	float barometric			{0};

    float waterVolume			{0};
    float waterDepth			{0};
    float waterFlow				{0};

	char sessionDirectory[32]	{0};
	char logFilepath[64]		{0};
	char statusFilepath[64]		{0};

    const char * stateName		{nullptr};
    int numberOfStates			{0};

    static KPStatus & current() {
        static KPStatus s;
        return s;
    }

	bool isProgrammingMode() {
		uint threshold = 100;
		uint rtc_pin = analogRead(RTC_Interrupt_Pin); 			// Active Low
		uint override_pin = analogRead(Override_Mode_Pin);		// Active Low
		Serial.println(rtc_pin, override_pin);
		return (rtc_pin <= threshold && override_pin <= threshold);
	}
private:
	KPStatus() {}
};

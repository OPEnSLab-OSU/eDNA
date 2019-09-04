#pragma once
#include "KPComponent.hpp"
#include "KPFoundation.hpp"
#include "KPPowerManagement.hpp"
#include "KPSDCard.hpp"

//===============================================================
// [+_+] External Devices
//===============================================================
extern KPSDCard card;
extern KPPowerManagement power;

class KPLogging : public KPComponent {
public:
    // KPSDCard & card;
    // KPPowerManagement & power;

    // KPLogging(KPSDCard & card, KPPowerManagement & power)
    // : KPComponent(), card(card), power(power) {

    // }

    void logCurrentStatusCSVFormatted() {
        auto & s = KPStatus::current();
        char * logFilepath = s.logFilepath;
        if (!SD.exists(logFilepath)) {
            File file = SD.open(logFilepath, FILE_WRITE);
            file.println("UTC Time, State, Pressure, Temperature, Water Flow, Water Depth, Barometic");
            file.close();
			Serial.println("Log file created");
        }

        File file = SD.open(logFilepath, O_RDWR | O_APPEND);
		char row[1024];
		const char * formatter = "%u, %s, %.2f, %.2f, %.2f";
		sprintf(row, formatter, (long) now(), s.stateName, s.pressure, s.temperature, s.waterFlow);
		Serial.println(row);

		file.println(row);
		file.close();
		Serial.println("Finished logging to file");
        
		notify<void>(Event::LGFinishedLogging, nullptr);
    }
};
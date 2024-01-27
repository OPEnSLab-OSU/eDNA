#pragma once
#include <Components/Sensor.hpp>
#include <SSC.h>

class PressureSensor : public Sensor<float, float> {
private:
    SSC sensor;

    void begin() override {
        setUpdateFreq(3);
        sensor.setMinRaw(1000);
        sensor.setMaxRaw(15000);
        sensor.setMinPressure(0);
        sensor.setMaxPressure(100);
        sensor.start();
    };

    //formula is slightly different than what SSC library provides, so use instead
    //datasheet: https://media.digikey.com/pdf/Data%20Sheets/Tyco%20Electronics%20Measurements%20PDFs/M3200%20Datasheet.pdf
    float rawToTemperature(uint16_t raw) { return float(raw) * 0.09765625 - 50.0; }

public:
    PressureSensor(int addr) : sensor(addr) {}

    SensorData read() override {
        sensor.update();
        return {sensor.pressure(), rawToTemperature(sensor.temperature_Raw())};
    }
};

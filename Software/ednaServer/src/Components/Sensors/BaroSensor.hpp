#pragma once
#include <Components/Sensor.hpp>
#include <MS5803_02.h>

class BaroSensor : public Sensor<float, float> {
private:
    MS_5803 sensor;

    void begin() override {
        setUpdateFreq(3);
        sensor.initializeMS_5803(true);
    }

public:
    BaroSensor(byte address) : sensor(address, 512) {}
    SensorData read() override {
        return {sensor.pressure(), sensor.temperature()};
    }
};
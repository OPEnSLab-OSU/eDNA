#pragma once
#include <Components/Sensor.hpp>
#include <Application/Constants.hpp>

extern volatile unsigned long lastFlowTick;
extern volatile unsigned long flowIntervalMicros;
extern volatile bool flowUpdated;


void flowTick();

inline double interpolate(double x, double in_min, double in_max, double out_min, double out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

struct TurbineFlowSensorData {
    double volume;
    double mlpm;
   
};

class TurbineFlowSensor : public Sensor<TurbineFlowSensorData> {
private:
    void begin() override {
        lastFlowTick = micros();
        lastInterruptMicros = micros();
        pinMode(HardwarePins::ANALOG_SENSOR_1, INPUT);
        //sensor is updated once a second
        setUpdateFreq(1000);
    }

public:
    double volume = 0;
    double mlpm   = 0;
    volatile unsigned long lastInterruptMicros;
    void resetVolume() {
        volume = 0;
    }

    void startMeasurement() {
        //lastInterruptMicros = micros();
        attachInterrupt(digitalPinToInterrupt(HardwarePins::ANALOG_SENSOR_1), flowTick, FALLING);
    }

    void stopMeasurement() {
        detachInterrupt(digitalPinToInterrupt(HardwarePins::ANALOG_SENSOR_1));
    }

  
    SensorData read() override {
    // Get the current time
    unsigned long currentMicros = micros();
    
    if (flowUpdated) {
        flowUpdated = false;
        //micro is 1e-6, so we divide the micros in a second with the flow interval
        //flowInterval (minutes) = x ms * (1s/1e*6ms) * (1m/60s)
        auto flowInterval = double(flowIntervalMicros)/60000000;
        //Sensor Pulse Constant: 0.11 ml
        mlpm    = 0.11/flowInterval;
        //mlpm * change in minute gives volume in mililiters
        volume += 0.115;
        println("Volume: ", volume, ", mmlpm: ", mlpm);
    
        // Update the time of the last interrupt
        lastInterruptMicros = currentMicros;
  
    }
    else {
        // If more than 1 second (1000000 microseconds) has passed since the last interrupt, set flow rate to 0
       // setErrorCode(ErrorCode::notReady);
        if (currentMicros - lastInterruptMicros >= 1000000) {
            println("not reading from flow meter");
            mlpm = 0;
        }
        setErrorCode(ErrorCode::success);
    }

    return {volume, mlpm};
}

};
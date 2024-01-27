#pragma once
#include <Components/Sensor.hpp>

struct AnalogFlowSensorData {
    double volume;
    double lpm;
};

class AnalogFlowSensor : public Sensor<AnalogFlowSensorData> {
    

    void begin() override {
        //pinMode(ADDR, INPUT);
      //  Wire.begin();
        //sensor is updated once a second
        setUpdateFreq(1000);
    }

    double countToFlow(double volts) {
        double ml_per_min = 0;
        if (volts < .50) {
            ml_per_min = 0;
        } else if (volts < 2.39) {
            ml_per_min = 0.1054393305439331 * volts - 0.0527196652719665;
        } else if (volts < 2.94) {
            ml_per_min = 0.1704081632653061 * volts - 0.1552755102040816;
        } else if (volts < 3.21) {
            ml_per_min = 0.2330218068535826 * volts - 0.1840841121495327;
        } else if (volts < 3.47) {
            ml_per_min = 0.2956772334293948 * volts - 0.2011239193083573;
        }

        // currently voltage wouldn't go above 3.3, so don't need rest of graph

        return ml_per_min;
    }
    public:
        double volume = 0;
        double lpm    = 0;
        bool canMeasure = false;

        const int ADDR;
        AnalogFlowSensor(int addr) : ADDR(addr) {}

        void resetVolume() {
            volume = 0;
        }

        //these functions make sure volume is only recorded in sample state
        void startMeasurement() {
            canMeasure = true;
        }

        void stopMeasurement() {
            canMeasure = false;
        }

        SensorData read() override {
            if(!canMeasure){
                setErrorCode(ErrorCode::notReady);
                lpm = 0;
                return {volume, lpm};
            }
            
            // should returns [checksum, count-high, count-low, temp-high, temp-low]
        /* Wire.requestFrom(ADDR, 5, true);
            int checksum = Wire.read();
            int count_h  = Wire.read();
            int count_l  = Wire.read();
            int temp_h   = Wire.read();
            int temp_l   = Wire.read();

            byte check = checksum + count_h + count_l + temp_h + temp_l;
            if (check) {
                setErrorCode(ErrorCode::invalidChecksum);
            }*/
            
            lpm = countToFlow(interpolate(analogRead(ADDR), 0, 1023, 0, 3.3));
            volume += lpm / 60.0;
            return {volume, lpm};
        }


    
};
#pragma once
#include <Components/Sensor.hpp>

struct FlowSensorData {
    int flow;
    int temp;
};

struct FlowSensor : public Sensor<FlowSensorData> {
    const int ADDR;
    FlowSensor(int addr) : ADDR(addr) {}

    void begin() override {
        Wire.begin();
    }

    int countToFlow(int count) {
        double ml_per_min = 0;
        if (count < 409) {
            ml_per_min = 0;
        } else if (count < 1362) {
            ml_per_min = 0.079748163693599 * count - 23.61699895068206;
        } else if (count < 1403) {
            ml_per_min = 0.365853658536585 * count - 413.2926829268292;
        } else if (count < 1572) {
            ml_per_min = 0.314465408805031 * count - 315.0887573964497;
        } else if (count < 1761) {
            ml_per_min = 0.275132275132275 * count - 282.5079365079365;
        } else if (count < 2103) {
            ml_per_min = 0.295321637426901 * count - 318.0614035087719;
        } else if (count < 2353) {
            ml_per_min = 0.396 * count - 529.788;
        } else if (count < 2535) {
            ml_per_min = 0.554945054945055 * count - 903.7857142857140;
        } else if (count < 2650) {
            ml_per_min = 0.860869565217391 * count - 1679.304347826087;
        } else if (count < 2715) {
            ml_per_min = 1.538461538461539 * count - 3474.923076923077;
        }

        return static_cast<int>(ml_per_min);
    }

    SensorData read() override {
        // should returns [checksum, count-high, count-low, temp-high, temp-low]
        Wire.requestFrom(ADDR, 5, true);
        int checksum = Wire.read();
        int count_h  = Wire.read();
        int count_l  = Wire.read();
        int temp_h   = Wire.read();
        int temp_l   = Wire.read();

        byte check = checksum + count_h + count_l + temp_h + temp_l;
        if (check) {
            setErrorCode(ErrorCode::invalidChecksum);
        }

        return {countToFlow((count_h << 8) | count_l), (temp_h << 8) | temp_l};
    }
};
// The Curiously Recurring Template Pattern (CRTP) allows us to build functionalities
// without dynamic dispatch.
// See: https://www.fluentcpp.com/2017/05/12/curiously-recurring-template-pattern/

// template <typename T>
// class Crtp {
// protected:
// 	T & underlying() {
// 		return static_cast<T &>(*this);
// 	}
// 	T const & underlying() const {
// 		return static_cast<T const &>(*this);
// 	}
// };

// template <typename SensorType>
// class I2CSensor : public Crtp<SensorType> {
// private:
// 	friend SensorType;
// 	I2CSensor() {}

// public:
// 	float updateFreqHz				  = 2;
// 	unsigned long timeSinceLastUpdate = 0;
// 	unsigned char addr				  = 0;
// 	unsigned char enabled			  = false;
// 	unsigned char updated			  = false;

// 	void updateSensor() {
// 		if (!enabled) {
// 			return;
// 		}

// 		if ((millis() - timeSinceLastUpdate) >= 1000 / updateFreqHz) {
// 			this->underlying().update();
// 			updated				= true;
// 			timeSinceLastUpdate = millis();
// 		}
// 	}

// 	bool didUpdate() {
// 		if (updated) {
// 			updated = false;
// 			return true;
// 		} else {
// 			return false;
// 		}
// 	}
// };

#pragma once
#include <vector>
#include <KPSubject.hpp>
#include <Components/SensorArrayObserver.hpp>

#include <Components/Sensors/TurbineFlowSensor.hpp>
#include <Components/Sensors/PressureSensor.hpp>
#include <Components/Sensors/BaroSensor.hpp>
#include <Components/Sensors/AnalogFlowSensor.hpp>
#include <Application/Constants.hpp>

#define PSAddr 0x28
#define FSAddr 0x07
#define BSAddr 0x77
#define DSAddr 0x76

inline bool checkForI2CConnection(unsigned char addr) {
    Wire.begin();
    Wire.requestFrom(addr, 1);
    return Wire.read() != -1;
}

class SensorArray : public KPComponent, public KPSubject<SensorArrayObserver> {
public:
    using KPComponent::KPComponent;

    TurbineFlowSensor flow;
    //AnalogFlowSensor flow{HardwarePins::ANALOG_SENSOR_1};
    PressureSensor pressure{PSAddr};
    BaroSensor baro1{BSAddr};
    BaroSensor baro2{DSAddr};

    void setup() override {
        flow.enabled    = true;
        flow.onReceived = [this](TurbineFlowSensor::SensorData & data) {
            updateObservers(&SensorArrayObserver::flowSensorDidUpdate, data);
        };

        pressure.enabled    = checkForI2CConnection(PSAddr);
        pressure.onReceived = [this](PressureSensor::SensorData & data) {
            updateObservers(&SensorArrayObserver::pressureSensorDidUpdate, data);
        };
        baro1.enabled    = checkForI2CConnection(BSAddr);
        baro1.onReceived = [this](BaroSensor::SensorData & data) {
            updateObservers(&SensorArrayObserver::baro1DidUpdate, data);
        };

        baro2.enabled    = checkForI2CConnection(DSAddr);
        baro2.onReceived = [this](BaroSensor::SensorData & data) {
            updateObservers(&SensorArrayObserver::baro2DidUpdate, data);
        };
    }

    void update() override {
        flow.update();
        pressure.update();
        baro1.update();
        baro2.update();
    }
};
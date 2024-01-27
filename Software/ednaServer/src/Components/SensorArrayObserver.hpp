#pragma once
#include <KPObserver.hpp>
#include <Components/Sensors/TurbineFlowSensor.hpp>
#include <Components/Sensors/PressureSensor.hpp>
#include <Components/Sensors/BaroSensor.hpp>
#include <Components/Sensors/AnalogFlowSensor.hpp>

class SensorArrayObserver : public KPObserver {
public:
    const char * ObserverName() const {
        return SensorManagerObserverName();
    }

    virtual const char * SensorManagerObserverName() const = 0;

    virtual void flowSensorDidUpdate(TurbineFlowSensor::SensorData & values, const KPState * current) {}
    virtual void analogFlowSensorDidUpdate(AnalogFlowSensor::SensorData & values) {}
    virtual void pressureSensorDidUpdate(PressureSensor::SensorData & values) {}
    virtual void baro1DidUpdate(BaroSensor::SensorData & values) {}
    virtual void baro2DidUpdate(BaroSensor::SensorData & values) {}
};

#pragma once
#include "KPFoundation.hpp"
#include "KPComponent.hpp"
#include "SSC.h"
#include "MS5803_02.h"

#define PSAddr 0x08
#define FSAddr 0x07
#define BSAddr 0x77
#define DSAddr 0x76

class I2CSensor {
public:
	int addr;
	bool enabled = false;

	I2CSensor(int addr) : addr(addr) {}

	virtual void checkForConnection() {
		Wire.begin();
		Wire.beginTransmission(addr);
		
		Wire.write(1);
		Wire.endTransmission();
		Wire.requestFrom(addr, 1);
		enabled = (Wire.read() != -1);
	}

	virtual void setup() {}
};

class PressureSensor : public I2CSensor {
public:
	SSC sensor;
	PressureSensor() : I2CSensor(PSAddr), sensor(PSAddr) {}

	void setup() override {
		sensor.setMinRaw(1638);      
		sensor.setMaxRaw(14745);    
		sensor.setMinPressure(0);   
		sensor.setMaxPressure(30);  
		sensor.start();
	}
};

class FlowSensor : public I2CSensor {
public:
	FlowSensor() : I2CSensor(FSAddr) {}

	int flow() {
		Wire.requestFrom(FSAddr, 2);
		byte a = Wire.read();
		byte b = Wire.read();
		int flow = ((a << 8) + b) / 10;
		return flow;
	}
};

// class BaroSensor : public I2CSensor {
// public:
// 	MS_5803 sensor;
// 	BaroSensor() : I2CSensor(BSAddr), sensor(BSAddr, 512) {
// 		Wire.begin();
// 		sensor.initializeMS_5803();
// 	}
// };


MS_5803 barometric(BSAddr, 1024);
MS_5803 depth(DSAddr, 1024);

class KPSensorManager : public KPComponent {
public:
	PressureSensor ps;
	FlowSensor fs;
	// BaroSensor bs;

	void setup() override {
		ps.checkForConnection();
		fs.checkForConnection();
		// bs.checkForConnection();

		if (ps.enabled) {
			ps.setup(); 
		}

		if (fs.enabled) {
			fs.setup();
		}

		barometric.initializeMS_5803();
		depth.initializeMS_5803();
	}

	void update() override {
		KPStatus & s = KPStatus::current();

		if (ps.enabled) {
			ps.sensor.update();
			s.pressure = ps.sensor.pressure();
			s.temperature = ps.sensor.temperature();
		}

		if (fs.enabled) {
			s.waterFlow = fs.flow();
		}

		barometric.readSensor();
		depth.readSensor();
		s.barometric = barometric.pressure() + 14;
		s.waterDepth = (depth.pressure() - s.barometric) * 0.010207;
	}
};
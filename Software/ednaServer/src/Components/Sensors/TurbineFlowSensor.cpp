#include <Components/Sensors/TurbineFlowSensor.hpp>

volatile unsigned long lastFlowTick;
volatile unsigned long flowIntervalMicros;
volatile bool flowUpdated;

void flowTick() {
	//This is the time interval since the last flowTick
	flowIntervalMicros = micros() - lastFlowTick;
	lastFlowTick	   = micros();
	//make sure to read data
	flowUpdated		   = true;
}
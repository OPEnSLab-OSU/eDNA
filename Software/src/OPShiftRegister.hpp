//
//  OPShiftRegister.cpp
//  OpenSamplerFramework
//
//  Created by Kawin on 2/10/19.
//  Copyright © 2019 Kawin. All rights reserved.
//

#pragma once
#include "OPComponent.hpp"
#include "SPI.h"

class OPShiftRegister: public OPComponent {
public:
    byte * outputs;
    byte registersCount;

    byte capacity;
    byte capacityPerRegister = 8;
    
    int dataPin  = 0;
    int clockPin = 0;
    int latchPin = 0;

	BitOrder bitOrder = MSBFIRST;
public:

	int registerIndex(int value);
	int bitIndex(int value);

    OPShiftRegister(const char * name, byte capacity) : OPComponent(name), capacity(capacity) {
        registersCount = capacity / capacityPerRegister;
        outputs = new byte[registersCount]();
    }

    OPShiftRegister(const char * name, byte capacity, byte data, byte clock, byte latch) : 
	OPShiftRegister(name, capacity) {
        setPins(data, clock, latch);
    }

	void setPins(byte data, byte clock, byte latch);

    void setZeros();
    void setOnes();

	void setRegister(byte index, byte bitNumber, bool signal);
	void setRegister(byte index, byte value);

    void writeOneHot(byte bitToSet);
	void writeZeros();

	void flush();
};
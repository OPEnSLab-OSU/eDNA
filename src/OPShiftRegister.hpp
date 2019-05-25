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

    OPShiftRegister(String name, byte capacity) : OPComponent(name), capacity(capacity) {
        registersCount = capacity / capacityPerRegister;
        outputs = new byte[registersCount];
    }

    OPShiftRegister(String name, byte capacity, byte data, byte clock, byte latch) : 
	OPShiftRegister(name, capacity) {
        setPins(data, clock, latch);
    }

    void setPins(byte data, byte clock, byte latch) {
        dataPin = data;
        clockPin = clock;
        latchPin = latch;

        pinMode(dataPin, OUTPUT);
        pinMode(clockPin, OUTPUT);
        pinMode(latchPin, OUTPUT);
    }

    void setZeros() {
        for (int i = 0; i < registersCount; i++){
            outputs[i] = 0;
        }
    }

    void setOnes() {
        for (int i = 0; i < registersCount; i++){
            outputs[i] = 255;
        }
    }

	void setRegister(byte index, byte bitNumber, bool signal) {
		if (index >= registersCount || bitNumber >= capacityPerRegister) {
			return;
		}

		if (signal) {
			outputs[index] |= (1 << bitNumber);
		} else {
			outputs[index] &= ~(1 << bitNumber);
		}
	}

	void setRegister(byte index, byte value) {
		if (index >= registersCount) {
			return;
		}

		outputs[index] = value;
	}

	void flush() {
        digitalWrite(latchPin, LOW);
        for (int i = registersCount - 1; i >= 0; i--){
            shiftOut(dataPin, clockPin, bitOrder, outputs[i]);
        }
        digitalWrite(latchPin, HIGH);
    }

    void writeOneHot(byte bitToSet) {
        if (bitToSet / capacityPerRegister >= registersCount) {
            return;
        }

        setZeros();
        byte index = bitToSet / capacityPerRegister;
		byte bitNumber = 1 << (bitToSet % capacityPerRegister);
        outputs[index] = bitNumber;
        flush();
    }

	void writeZero() {
		setZeros();
		flush();
	}
};
//
//  KPShiftRegister.cpp
//  eDNA Framework
//
//  Created by Kawin on 2/10/19.
//  Copyright © 2019 Kawin. All rights reserved.
//

#pragma once
#include "KPFoundation.hpp"
#include "KPComponent.hpp"
#include "SPI.h"

class KPShiftRegister : public KPComponent {
public:
    byte * outputs;

    const byte capacityPerRegister = 8;
    const byte capacity;
    const byte registersCount;

    int dataPin = 0;
    int clockPin = 0;
    int latchPin = 0;

    BitOrder bitOrder = MSBFIRST;
public:
	int registerIndex(int value) { return value / capacityPerRegister; }
    int bitIndex(int value) { return value % capacityPerRegister;}

    KPShiftRegister(const char * name, const byte capacity)
        : KPComponent(name), capacity(capacity), registersCount(capacity / capacityPerRegister) {
        outputs = new byte[registersCount]();
    }

    KPShiftRegister(const char * name, byte capacity, byte data, byte clock, byte latch)
        : KPShiftRegister(name, capacity) {
        setPins(data, clock, latch);
    }

	KPShiftRegister(byte capacity, byte data, byte clock, byte latch) 
	: KPShiftRegister("", capacity, data, clock, latch) {}
    

    void setPins(byte data, byte clock, byte latch) {
        dataPin = data;
        pinMode(dataPin, OUTPUT);
        clockPin = clock;
        pinMode(clockPin, OUTPUT);
        latchPin = latch;
        pinMode(latchPin, OUTPUT);
        writeZeros();
    }

    void setZeros() {
        for (int i = 0; i < registersCount; i++) {
            outputs[i] = 0;
        }
    }

    void setOnes() {
        for (int i = 0; i < registersCount; i++) {
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
        for (int i = registersCount - 1; i >= 0; i--) {
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

    void writeZeros() {
        setZeros();
        flush();
    }

	void writeLatchIn() {
		setRegister(0, 0, HIGH);  // Latch Valve
        flush();
        delay(100);
        setRegister(0, 0, LOW);
        flush();
	}

	void writeLatchOut() {
		setRegister(0, 1, HIGH);  // Latch Valve
        flush();
        delay(100);
        setRegister(0, 1, LOW);
        flush();
	}

	void set(byte index, bool signal) {
		setRegister(index / capacityPerRegister, index % capacityPerRegister, signal);
	}
};
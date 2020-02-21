//
//  KPShiftRegister.cpp
//  eDNA Framework
//
//  Created by Kawin on 2/10/19.
//  Copyright © 2019 Kawin. All rights reserved.
//

#pragma once
#include <KPFoundation.hpp>
#include <SPI.h>

class ShiftRegister : public KPComponent {
public:
    byte * outputs;

    const byte capacityPerRegister = 8;
    const byte capacity;
    const byte registersCount;

    int dataPin  = 0;
    int clockPin = 0;
    int latchPin = 0;

    BitOrder bitOrder = MSBFIRST;

public:
    int toRegisterIndex(int value) { return value / capacityPerRegister; }
    int toBitIndex(int value) { return value % capacityPerRegister; }

    ShiftRegister(const char * name, const byte capacity)
        : KPComponent(name), capacity(capacity), registersCount(capacity / capacityPerRegister) {
        outputs = new byte[registersCount]();
    }

    ShiftRegister(const char * name, byte capacity, byte data, byte clock, byte latch)
        : ShiftRegister(name, capacity) {
        setPins(data, clock, latch);
    }

    ShiftRegister(byte capacity, byte data, byte clock, byte latch)
        : ShiftRegister("", capacity, data, clock, latch) {}

    void flush() {
        digitalWrite(latchPin, LOW);
        for (int i = registersCount - 1; i >= 0; i--) {
            shiftOut(dataPin, clockPin, bitOrder, outputs[i]);
        }
        digitalWrite(latchPin, HIGH);
    }

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

    void writeZeros() {
        setZeros();
        flush();
    }

    void setOnes() {
        for (int i = 0; i < registersCount; i++) {
            outputs[i] = 255;
        }
    }

    void writeOnes() {
        setOnes();
        flush();
    }

    void setRegister(byte registerIndex, byte bitIndex, bool signal) {
        if (registerIndex >= registersCount || bitIndex >= capacityPerRegister) {
            return;
        }

        if (signal) {
            outputs[registerIndex] |= (1 << bitIndex);
        } else {
            outputs[registerIndex] &= ~(1 << bitIndex);
        }
    }

    void writeRegister(byte registerIndex, byte bitIndex, bool signal) {
        setRegister(registerIndex, bitIndex, signal);
        flush();
    }

    void setBit(byte index, bool signal) {
        if (index < 0 || index >= capacity) {
            return;
        }

        setRegister(toRegisterIndex(index), toBitIndex(index), signal);
    }

    void writeBit(byte index, bool signal) {
        setBit(index, signal);
        flush();
    }

    void writeOneHot(byte bitIndex) {
        if (bitIndex / capacityPerRegister >= registersCount) {
            return;
        }

        setZeros();
        byte bitNumber                     = 1 << (toBitIndex(bitIndex));
        outputs[toRegisterIndex(bitIndex)] = bitNumber;
        flush();
    }

private:
    void writeLatch(bool out) {
        setRegister(0, out, HIGH);  // Latch Valve
        flush();
        delay(80);
        setRegister(0, out, LOW);
        flush();
    }

public:
    void writeLatchIn() {
        writeLatch(0);
    }

    void writeLatchOut() {
        writeLatch(1);
    }
};
#include "OPShiftRegister.hpp"

int OPShiftRegister::registerIndex(int value) {
    return value / capacityPerRegister;
}

int OPShiftRegister::bitIndex(int value) {
    return value % capacityPerRegister;
}

void OPShiftRegister::setPins(byte data, byte clock, byte latch) {
    dataPin  = data;  pinMode(dataPin, OUTPUT);
    clockPin = clock; pinMode(clockPin, OUTPUT);
    latchPin = latch; pinMode(latchPin, OUTPUT);
	writeZeros();
}

void OPShiftRegister::setZeros() {
    for (int i = 0; i < registersCount; i++) {
        outputs[i] = 0;
    }
}

void OPShiftRegister::setOnes() {
    for (int i = 0; i < registersCount; i++) {
        outputs[i] = 255;
    }
}

void OPShiftRegister::setRegister(byte index, byte bitNumber, bool signal) {
    if (index >= registersCount || bitNumber >= capacityPerRegister) {
        return;
    }

    if (signal) {
        outputs[index] |= (1 << bitNumber);
    } else {
        outputs[index] &= ~(1 << bitNumber);
    }
}

void OPShiftRegister::setRegister(byte index, byte value) {
    if (index >= registersCount) {
        return;
    }

    outputs[index] = value;
}

void OPShiftRegister::flush() {
    digitalWrite(latchPin, LOW);
    for (int i = registersCount - 1; i >= 0; i--) {
        shiftOut(dataPin, clockPin, bitOrder, outputs[i]);
    }
    digitalWrite(latchPin, HIGH);
}

void OPShiftRegister::writeOneHot(byte bitToSet) {
    if (bitToSet / capacityPerRegister >= registersCount) {
        return;
    }

    setZeros();
    byte index = bitToSet / capacityPerRegister;
    byte bitNumber = 1 << (bitToSet % capacityPerRegister);
    outputs[index] = bitNumber;
    flush();
}

void OPShiftRegister::writeZeros() {
    setZeros();
    flush();
}
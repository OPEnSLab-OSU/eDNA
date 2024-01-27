/**
 * Copyright (c) 2020 Kawin Pechetratanapanit
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once
#include <KPFoundation.hpp>
#include <SPI.h>

class ShiftRegister : public KPComponent {
public:
    const int capacityPerRegister = 8;
    const int registersCount;
    const int dataPin;
    const int clockPin;
    const int latchPin;

    int8_t * registers;
    BitOrder bitOrder = MSBFIRST;

public:
    ShiftRegister(const char * name, int registerCount, int data, int clock, int latch)
        : KPComponent(name),
          registersCount(registerCount),
          dataPin(data),
          clockPin(clock),
          latchPin(latch) {
        registers = new int8_t[registersCount]();
        setRegisterPins(data, clock, latch);
    }

    void setup() override {
        writeAllRegistersLow();
    }

    /** ────────────────────────────────────────────────────────────────────────────
     *  @brief Set all given pins to output mode. The registers should be hooked up
     *  in daisy chain mode.
     *
     *  @param data Data pin
     *  @param clock Clock pin
     *  @param latch Latch pin
     *  ──────────────────────────────────────────────────────────────────────────── */
    void setRegisterPins(int data, int clock, int latch) {
        pinMode(dataPin, OUTPUT);
        pinMode(clockPin, OUTPUT);
        pinMode(latchPin, OUTPUT);
    }

    /** ────────────────────────────────────────────────────────────────────────────
     *  Helper converting 1D pin number to register index.
     *
     *  @param pinNumber Pin number (ex: 0,1,2,...,23)
     *  @return int Index of the register containing this pin
     *  ──────────────────────────────────────────────────────────────────────────── */
    auto toRegisterIndex(int pinNumber) const -> int {
        return pinNumber / capacityPerRegister;
    }

    /** ────────────────────────────────────────────────────────────────────────────
     *  Helper converting 1D pin number to the pin index of the register containing this
     *  pin
     *
     *  @param pinNumber Pin number (ex: 0,1,2,...,23)
     *  @return int Index of the pin
     *  ──────────────────────────────────────────────────────────────────────────── */
    auto toPinIndex(int pinNumber) const -> int {
        return pinNumber % capacityPerRegister;
    }

    auto toRegisterAndPinIndices(int pinNumber) const -> std::pair<int, int> {
        return {toRegisterIndex(pinNumber), toPinIndex(pinNumber)};
    }

    void setAllRegistersLow() {
        for (int i = 0; i < registersCount; i++) {
            registers[i] = 0;
        }
    }

    void setAllRegistersHigh() {
        for (int i = 0; i < registersCount; i++) {
            registers[i] = 0xFF;
        }
    }

    /** ────────────────────────────────────────────────────────────────────────────
     *  Set individual pin of the register high/low given the register index
     *
     *  @param registerIndex Index of the register
     *  @param pinIndex Index of the pin
     *  @param signal HIGH or LOW
     *  ──────────────────────────────────────────────────────────────────────────── */
    void setRegister(int registerIndex, int pinIndex, bool signal) {
        if (registerIndex >= registersCount || pinIndex >= capacityPerRegister) {
            return;
        }

        if (signal) {
            registers[registerIndex] |= (1 << pinIndex);
        } else {
            registers[registerIndex] &= ~(1 << pinIndex);
        }
    }

    /** ────────────────────────────────────────────────────────────────────────────
     *  Set invidual pin of the register high/low
     *
     *  @param number Pin number
     *  @param signal HIGH or LOW
     *  ──────────────────────────────────────────────────────────────────────────── */
    void setPin(int number, bool signal) {
        if (number < 0 || number >= registersCount * capacityPerRegister) {
            return;
        }

        setRegister(toRegisterIndex(number), toPinIndex(number), signal);
    }

    /** ────────────────────────────────────────────────────────────────────────────
     *  Shiftout each byte in the register array in reverse order
     *
     *  ──────────────────────────────────────────────────────────────────────────── */
    void write() {
        digitalWrite(latchPin, LOW);
        for (int i = registersCount - 1; i >= 0; i--) {
            shiftOut(dataPin, clockPin, bitOrder, registers[i]);
        }
        digitalWrite(latchPin, HIGH);
    }

    void writePin(int index, bool signal) {
        setPin(index, signal);
        write();
    }

    void writeRegister(int registerIndex, int pinIndex, bool signal) {
        setRegister(registerIndex, pinIndex, signal);
        write();
    }

    void writeAllRegistersLow() {
        setAllRegistersLow();
        write();
    }

    void writeAllRegistersHigh() {
        setAllRegistersHigh();
        write();
    }

    /** ────────────────────────────────────────────────────────────────────────────
     *  Turn on a single pin while keeping the rest low
     *
     *  @param pinNumber
     *  @ref One-hot: https://en.wikipedia.org/wiki/One-hot
     *  ──────────────────────────────────────────────────────────────────────────── */
    void writeOneHot(int pinNumber) {
        if (toRegisterIndex(pinNumber) >= registersCount) {
            return;
        }

        setAllRegistersLow();
        setPin(pinNumber, HIGH);
        write();
    }
};
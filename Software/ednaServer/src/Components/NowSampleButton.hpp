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
#include <functional>

#include <Application/Constants.hpp>


//
// ──────────────────────────────────────────────────────────────────────── I ──────────
//   :::::: B U T T O N   M A N A G E M E N T : :  :   :    :     :        :          :
// ──────────────────────────────────────────────────────────────────────────────────
//
// 
//

extern volatile unsigned long buttonInterruptStart;
extern volatile bool buttonTriggered;
extern void button_isr();

class NowSampleButton : public KPComponent {
public:
    std::function<void()> interruptCallback;

    NowSampleButton(const char * name) : KPComponent(name) {}

    void onInterrupt(std::function<void()> callbcak) {
        interruptCallback = callbcak;
    }

    void setupButton() {
        pinMode(HardwarePins::BUTTON_PIN, INPUT_PULLUP);
    }

    void setup() override {
        setupButton();
    }

    /** ────────────────────────────────────────────────────────────────────────────
     *  Runtime update loop. Check if Button has been triggered.
     *  ──────────────────────────────────────────────────────────────────────────── */
    void update() override {
        if (!buttonTriggered || !interruptCallback) {
            return;
        }

        // Continue if button has a new press
        disableSampleButton();
        noInterrupts();
        buttonTriggered = false;
        interruptCallback();
    }



 

    /** ────────────────────────────────────────────────────────────────────────────
     *  Disable button
     *
     *  ──────────────────────────────────────────────────────────────────────────── */
    void disableSampleButton() {
        println("Disabled sample now button");
        buttonTriggered = false;
        detachInterrupt(digitalPinToInterrupt(HardwarePins::BUTTON_PIN));
    }




    /** ────────────────────────────────────────────────────────────────────────────
     *  Set Button to be able to be pressed again
     *  ──────────────────────────────────────────────────────────────────────────── */
    void setSampleButton() {
        println("interrupt attached");
        buttonInterruptStart = millis();
        attachInterrupt(digitalPinToInterrupt(HardwarePins::BUTTON_PIN), button_isr, FALLING);
    }



};
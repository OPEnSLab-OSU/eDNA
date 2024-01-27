/*
 * Copyright 2020 Kawin Pechetratanapanit
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This file is only used for compilation tesitng and not intended
 * to represent an actual project structure. For exmaples of how to
 * use this framework, please refer to eDNA or OPEnSampler2 [+_+]
 */

#include <Action.hpp>
#include <KPApplicationRuntime.hpp>
#include <KPController.hpp>
#include <KPDataStoreInterface.hpp>
#include <KPFoundation.hpp>
#include <KPFileLoader.hpp>
#include <KPSerialInput.hpp>
#include <KPServer.hpp>
#include <KPServerRequest.hpp>
#include <KPServerResponse.hpp>
#include <KPStateMachine.hpp>
#include <KPSerialInputObserver.hpp>

#define Power_Module_Pin	A0
#define RTC_Interrupt_Pin	A1
#define Battery_Voltage_Pin A2
#define Analog_Sensor_1_Pin A3
#define Analog_Sensor_2_Pin A4
#define Override_Mode_Pin	A5
#define Motor_Forward_Pin	5
#define Motor_Reverse_Pin	6
#define SR_Latch_Pin		9
#define SDCard_Pin			10
#define SR_Clock_Pin		11
#define SR_Data_Pin			12
#define Button_Pin			13

// Application.hpp
class Application : public KPController, public KPSerialInputObserver {
private:
	KPServer web{"Web Server", "eDNA-test", "password"};
	KPStateMachine sm{"state-machine"};
	KPFileLoader card{"sd-card", SDCard_Pin};

	void commandReceived(const char * line) override {
		println(line);
	}

public:
	void setup() override {
		Serial.begin(115200);
		delay(3000);

		KPSerialInput::sharedInstance().addObserver(*this);
	}

	void update() override {}
} app;

// main.cpp
void setup() {
	Runtime::setInitialAppController(app);
}

void loop() {
	Runtime::update();
	ActionScheduler::sharedInstance().update();
}

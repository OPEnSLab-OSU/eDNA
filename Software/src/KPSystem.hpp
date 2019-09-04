#pragma once
#include "KPFoundation.hpp"
#include "KPComponent.hpp"
#include "KPStateMachine.hpp"
#include "KPTaskManager.hpp"
#include "KPActionScheduler.hpp"

class KPSystem : public KPStateMachine, public KPTaskManager {
private:
    VoidFunctionPointer client_prepare 		{nullptr};
    VoidFunctionPointer client_ready 		{nullptr};
    VoidFunctionPointer client_loop 		{nullptr};
public:
    Array<KPComponent *> components			{20};
    KPActionScheduler scheduler;

	KPSystem(VoidFunctionPointer _prepare, VoidFunctionPointer _ready, VoidFunctionPointer _loop, KPSDCard & card) 
	: KPStateMachine(), KPTaskManager(card) {
		this->client_prepare 	= _prepare;
		this->client_ready 		= _ready;
		this->client_loop 		= _loop;
	};

	void transitionTo(KPStateIdentifier id) override {
		KPStateMachine::transitionTo(id);
		updateCurrentTaskState(currentState->name);
	}

	void transitionTo(const char * name) override {
		KPStateMachine::transitionTo(name);
		updateCurrentTaskState(name);
	}

    void addComponent(KPComponent * c) {
    	components.append(c);
	}

	KPComponent * getComponent(const char * name) {
		for (int i = 0; i < components.size; i++) {
			auto c = components.get(i);
			if (strcmp(c->name, name) == 0) {
				return c;
			}
		}

		return nullptr;
	}

	void _setup() {
		client_prepare();
		Serial.println("Function called: prepare()");
		for (int i = 0; i < components.size; i++) {
			auto c = components.get(i);
			c->setup();
			c->ready = c->checkForConnection();
		}
		Serial.println("Components are ready to be used");
		client_ready();
		Serial.println("Function called: ready()");
	}

	void _update() {
		// Serial.println("Calling state machine update");
		updateStateMachine();
		// Serial.println("Calling scheduler update");
		scheduler.update();
		// Serial.println("Calling loop update);
		client_loop();
		// Serial.println("Calling components updates);
		for (int i = 0; i < components.size; i++) {
			components.get(i)->update();
		}
	}
};

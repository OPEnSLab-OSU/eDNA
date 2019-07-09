#include "OPSystem.hpp"

OPSystem::OPSystem(VoidFunctionPointer _prepare, VoidFunctionPointer _ready, VoidFunctionPointer _loop) 
	: stateNodes(), scheduler() {
    this->client_prepare = _prepare;
    this->client_ready = _ready;
    this->client_loop = _loop;
};

StateNode * OPSystem::getCurrentState() {
    return currentStateNode;
}

const char * OPSystem::getCurrentStateName() {
    return currentStateNode->state->name;
}

bool OPSystem::isInitialTransition() {
    return callCountSinceLastTransition() == 0;
}

bool OPSystem::isPendingState() {
    return transitionPending;
}

unsigned long OPSystem::timeSinceLastTransition() {
    return (unsigned long) (millis() - currentStateNode->startTime);
}

int OPSystem::callCountSinceLastTransition() {
    return currentStateNode->count;
}

void OPSystem::addComponent(OPComponent * c) {
    components.append(c);
}

OPComponent * OPSystem::getComponent(const char * name) {
    for (int i = 0; i < components.size; i++) {
        auto c = components.get(i);
        if (strcmp(c->name, name) == 0) {
            return c;
        }
    }

    return nullptr;
}

void OPSystem::_setup() {
    client_prepare();
    for (int i = 0; i < components.size; i++) {
        components.get(i)->setup();
        components.get(i)->ready = true;
    }
    client_ready();
}

void OPSystem::state_update() {
    if (currentStateNode == nullptr) {
        return;
    }

    if (transitionPending) {
        currentStateNode->begin();
        transitionPending = false;
    }

    currentStateNode->update(*this);
}

void OPSystem::_update() {
    state_update();
    scheduler.update();
    client_loop();
    for (int i = 0; i < components.size; i++) {
        components.get(i)->update();
    }
}
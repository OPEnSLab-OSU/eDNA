#pragma once
#include "OPSystem.hpp"

extern OPSystem app;

extern "C" char *sbrk(int i);
 
int free_ram () {
  char stack_dummy = 0;
  return &stack_dummy - sbrk(0);
}

void printFreeRam() {
	Serial.print("Free Memory: ");
	Serial.println(free_ram());
}

void run(const OPTask & task) {
    app.scheduler.append(task);
}

void runForever(OPTask & task) {
    task.repeat = -1;
    app.scheduler.append(task);
}

bool containsTask(const char * name) {
    return app.scheduler.contains(name);
}

void setTimeout(long ms, VoidFunctionPointer callback) {
    OPTask task;
    task.wait(ms, callback);
    run(task);
}

void addComponent(OPComponent * c) {
    app.addComponent(c);
}

void addComponent(OPComponent & c) {
	app.addComponent(&c);
}

void setup() {
    app._setup();
}

void loop() {
    app._update();
}
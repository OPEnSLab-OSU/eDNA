#pragma once
#include "KPSystem.hpp"

extern KPSystem app;

double map(double x, double in_min, double in_max, double out_min, double out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void run(const KPSequentialAction & sa) {
    app.scheduler.append(sa);
}

void runForever(KPSequentialAction & sa) {
    sa.repeat = -1;
    app.scheduler.append(sa);
}

bool contains(const char * name) {
    return app.scheduler.contains(name);
}

void setTimeout(long ms, VoidFunctionPointer callback) {
    KPSequentialAction sa;
    sa.wait(ms, callback);
    run(sa);
}

void addComponent(KPComponent * c) {
    app.addComponent(c);
}

void addComponent(KPComponent & c) {
	app.addComponent(&c);
}

void setup() {
    app._setup();
}

void loop() {
    app._update();
}
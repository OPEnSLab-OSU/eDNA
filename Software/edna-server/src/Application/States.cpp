#include <Application/Application.hpp>
#include <Application/States.hpp>

void StateIdle::enter(KPStateMachine & sm) {
}

void StateStop::enter(KPStateMachine & sm) {
	Application & app = *static_cast<Application *>(sm.controller);
	app.pump.off();
	app.shift.writeZeros();
	app.shift.writeLatchOut();

	// app.status.setCurrentValveStatus(ValveStatus::sampled);
	// TODO: Task update here

	sm.transitionTo(StateName::IDLE);
}

void StateDecon::pressurizeTo(float target) {
}

void StateDecon::enter(KPStateMachine & sm) {
	Application & app = *static_cast<Application *>(sm.controller);

	app.pump.off();
	app.shift.writeLatchOut();
	app.shift.writeZeros();

	// Turn on the flush valve, the air intake, and the pump to clean out the main pipe
	app.shift.setBit(FlushValveBitIndex, HIGH);
	app.shift.setBit(AirValveBitIndex, HIGH);
	app.shift.flush();
	app.pump.on();
	// auto const flushMainPipe =

	// Turn the pump off at 10th second
	setTimeCondition(secsToMillis(10), [&]() {
		app.pump.off();
	});

	// Turn the pump on to 75% power at 12th second then close the flush valve
	setTimeCondition(secsToMillis(12), [&]() {
		app.pump.pwm(0.75);
		app.shift.setBit(FlushValveBitIndex, LOW);
		app.shift.flush();
	});

	auto const condition = [&]() {
		return timeSinceLastTransition() >= 12 && app.status.pressure >= 2;
	};

	setCondition(condition, [&]() {
		int r = app.shift.toRegisterIndex(app.status.valveCurrent) + 1;
		int b = app.shift.toBitIndex(app.status.valveCurrent);

		app.pump.off();
		app.shift.setRegister(r, b, HIGH);
		app.shift.flush();
		delay(50);

		sm.transitionTo(StateName::STOP);
	});
}

void StatePreserve::enter(KPStateMachine & sm) {
	Application & app = *static_cast<Application *>(sm.controller);

	int r = app.shift.toRegisterIndex(app.status.valveCurrent) + 1;
	int b = app.shift.toBitIndex(app.status.valveCurrent);

	app.shift.writeLatchOut();
	app.shift.writeZeros();
	app.shift.setBit(AlcoholValveBitIndex, HIGH);
	app.shift.setRegister(r, b, HIGH);
	app.shift.flush();
	app.pump.on();

	setTimeCondition(time, [&]() {
		sm.transitionTo(StateName::DECON);
	});
}

void StateDry::enter(KPStateMachine & sm) {
	Application & app = *static_cast<Application *>(sm.controller);

	int r = app.shift.toRegisterIndex(app.status.valveCurrent) + 1;
	int b = app.shift.toBitIndex(app.status.valveCurrent);

	app.shift.setZeros();
	app.shift.setBit(AirValveBitIndex, HIGH);
	app.shift.setRegister(r, b, HIGH);
	app.shift.flush();
	app.pump.on();

	setTimeCondition(time, [&]() {
		sm.transitionTo(StateName::PRESERVE);
	});
}

void StateSample::enter(KPStateMachine & sm) {
	// app.status.samplePressurePoint = 0;
	Application & app = *static_cast<Application *>(sm.controller);

	int r = app.shift.toRegisterIndex(app.status.valveCurrent) + 1;
	int b = app.shift.toBitIndex(app.status.valveCurrent);

	app.shift.setZeros();
	app.shift.writeLatchIn();
	app.shift.setRegister(r, b, HIGH);	// Filter
	app.shift.flush();
	app.pump.on();

	// This condition will be evaluated repeatedly until true then the callback will be executed
	auto const condition = [&]() {
		return timeSinceLastTransition() >= secsToMillis(time) || app.status.pressure >= pressure;
	};

	setCondition(condition, [&]() {
		app.shift.writeLatchOut();
		sm.transitionTo(StateName::DRY);
	});
}

void StateFlush::enter(KPStateMachine & sm) {
	Application & app = *static_cast<Application *>(sm.controller);

	app.shift.setZeros();
	app.shift.writeLatchIn();
	app.shift.setBit(FlushValveBitIndex, HIGH);
	app.shift.flush();
	app.pump.on();

	setTimeCondition(time, [&]() {
		sm.transitionTo(StateName::SAMPLE);
	});
}
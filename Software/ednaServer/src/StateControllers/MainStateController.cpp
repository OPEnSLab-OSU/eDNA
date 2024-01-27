#include <StateControllers/MainStateController.hpp>
#include <Application/App.hpp>

void Main::Idle::enter(KPStateMachine & sm) {
    auto & app = *static_cast<App *>(sm.controller);
    println(app.scheduleNextActiveTask().description());
};

void Main::Stop::enter(KPStateMachine & sm) {
    auto & app = *static_cast<App *>(sm.controller);
    app.pump.off();
    app.shift.writeAllRegistersLow();
    app.intake.off();
    app.sensors.flow.stopMeasurement();

    app.vm.setValveStatus(app.status.currentValve, ValveStatus::sampled);
    app.vm.writeToDirectory();

    auto currentTaskId = app.currentTaskId;
    if(currentTaskId){
        app.tm.advanceTask(currentTaskId);
        app.tm.writeToDirectory();
    }
    if(app.sampleNowActive){
        app.ntm.advanceTask();
        app.ntm.writeToDirectory();
    }
    app.currentTaskId       = 0;
    app.sampleNowActive = false;
    app.nowSampleButton.setSampleButton();
    app.status.currentValve = -1;
    sm.next();
}

#include <StateControllers/HyperFlushStateController.hpp>
#include <Application/App.hpp>
void NewStateController::setup() {
    // registerState(SharedStates::Flush(), FLUSH1, [this](int code) {
    // 	switch (code) {
    // 	case 0:
    // 		return transitionTo(OFFSHOOT_CLEAN_1);
    // 	default:
    // 		halt(TRACE, "Unhandled state transition");
    // 	}
    // });
    // ..or alternatively if state only has one input and one output

    registerState(SharedStates::Flush(), FLUSH_1, [this](int code) {
        //Pressure above system max, panic exit
        if (code == -1)
            return transitionTo(STOP);
        return transitionTo(OFFSHOOT_CLEAN_1);
    });
    registerState(SharedStates::OffshootClean(5), OFFSHOOT_CLEAN_1, [this](int code) {
        //Pressure above system max, panic exit
        if (code == -1)
            return transitionTo(STOP);
        return transitionTo(FLUSH_2);
    });
    registerState(SharedStates::Flush(), FLUSH_2, [this](int code) {
        //Pressure above system max, panic exit
        if (code == -1)
            return transitionTo(STOP);
        return transitionTo(SAMPLE);
    });
    registerState(SharedStates::Sample(), SAMPLE, [this](int code) {
        auto & app = *static_cast<App *>(controller);
        //app.sensors.flow.stopMeasurement();
        app.logAfterSample();

        switch (code) {
        //Normal Exit
        case 0:
            return transitionTo(DEPRESSURE);
        //Pressure above system max, panic exit
        case -1:
            return transitionTo(STOP);
        default:
            halt(TRACE, "Unhandled state transition: ", code);
        }
    });
    registerState(SharedStates::Depressure(10), DEPRESSURE, [this](int code) {
        //Pressure above system max, panic exit
        if (code == -1)
            return transitionTo(STOP);
        return transitionTo(PRESERVE_FLUSH);
    });
    registerState(SharedStates::AlcoholPurge(), PRESERVE_FLUSH, [this](int code) {
        //Pressure above system max, panic exit
        if (code == -1)
            return transitionTo(STOP);
        return transitionTo(PRESERVE);
    });
    registerState(SharedStates::Preserve(), PRESERVE, [this](int code) {
        //Pressure above system max, panic exit
        if (code == -1)
            return transitionTo(STOP);
        return transitionTo(FLUSH_3);
    });
    /*
     registerState(SharedStates::Idle(), IDLE, [this](int code) {
        //Pressure above system max, panic exit
        if (code == -1)
            return transitionTo(STOP);
        return transitionTo(FLUSH_3);
    });
    */
     registerState(SharedStates::Flush(), FLUSH_3, [this](int code) {
        //Pressure above system max, panic exit
        if (code == -1)
            return transitionTo(STOP);
        return transitionTo( OFFSHOOT_CLEAN_2);
    });
    registerState(SharedStates::OffshootClean(10), OFFSHOOT_CLEAN_2, [this](int code) {
        //Pressure above system max, panic exit
        if (code == -1)
            return transitionTo(STOP);
        return transitionTo(AIR_FLUSH);
    });
    registerState(SharedStates::AirFlush(), AIR_FLUSH, [this](int code) {
        //Pressure above system max, panic exit
        if (code == -1)
            return transitionTo(STOP);
        return transitionTo(STOP);
    });
    
    // Reusing STOP and IDLE states from MainStateController
    registerState(Main::Stop(), STOP, IDLE);
    registerState(Main::Idle(), IDLE);
};
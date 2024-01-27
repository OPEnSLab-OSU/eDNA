#include <StateControllers/HyperFlushStateController.hpp>
#include <Application/App.hpp>
void NowTaskStateController::setup() {
    // registerState(SharedStates::Flush(), FLUSH1, [this](int code) {
    // 	switch (code) {
    // 	case 0:
    // 		return transitionTo(OFFSHOOT_CLEAN_1);
    // 	default:
    // 		halt(TRACE, "Unhandled state transition");
    // 	}
    // });
    // ..or alternatively if state only has one input and one output

    registerState(SharedStates::Flush(), FLUSH_1, OFFSHOOT_CLEAN_1);
    registerState(SharedStates::OffshootClean(5), OFFSHOOT_CLEAN_1, FLUSH_2);
    registerState(SharedStates::Flush(), FLUSH_2, SAMPLE);
    registerState(SharedStates::Sample(), SAMPLE, [this](int code) {
        auto & app = *static_cast<App *>(controller);
        //app.sensors.flow.stopMeasurement();
        app.logAfterSample();

        switch (code) {
        case 0:
            return transitionTo(DEPRESSURE);
        default:
            halt(TRACE, "Unhandled state transition: ", code);
        }
    });
    registerState(SharedStates::Depressure(10), DEPRESSURE, PRESERVE_FLUSH);
    registerState(SharedStates::AlcoholPurge(), PRESERVE_FLUSH, PRESERVE);
    registerState(SharedStates::Preserve(), PRESERVE, FLUSH_3);
   // registerState(SharedStates::Idle(), IDLE,FLUSH_3);
    registerState(SharedStates::Flush(), FLUSH_3,OFFSHOOT_CLEAN_2);
    registerState(SharedStates::OffshootClean(10), OFFSHOOT_CLEAN_2, AIR_FLUSH);
    registerState(SharedStates::AirFlush(), AIR_FLUSH, STOP);
    
    // Reusing STOP and IDLE states from MainStateController
    registerState(Main::Stop(), STOP, IDLE);
    registerState(Main::Idle(), IDLE);
};
#pragma once
#include <Components/StateController.hpp>
#include <States/Shared.hpp>

namespace Main {
    STATE(STOP);
    STATE(IDLE);
    STATE(PRESERVE);
    STATE(DRY);
    STATE(SAMPLE);
    STATE(FLUSH);

    /**
     * This state checks if there is an active task and schedule it if any.
     * [Connections: 0]
     */
    class Idle : public KPState {
    public:
        void enter(KPStateMachine & sm) override;
    };

    /**
     * Turn everything off and make sure that anything changes to the valve manager and task
     * manager are committed to persistent store.
     * [Connections: 1]
     */
    class Stop : public KPState {
    public:
        void enter(KPStateMachine & sm) override;
    };

    struct Config {
        decltype(SharedStates::Flush::time) flushTime;
        decltype(SharedStates::Sample::time) sampleTime;
        decltype(SharedStates::Sample::pressure) samplePressure;
        decltype(SharedStates::Sample::volume) sampleVolume;
        decltype(SharedStates::Dry::time) dryTime;
        decltype(SharedStates::Preserve::time) preserveTime;
    };

    class Controller : public StateController, public StateControllerConfig<Config> {
    public:
        Controller() : StateController("main-state-machine") {}

        // FLUSH -> SAMPLE -> DRY -> PRESERVE -> STOP -> IDLE
        void setup() override {
            registerState(SharedStates::Flush(), FLUSH, SAMPLE);
            registerState(SharedStates::Sample(), SAMPLE, DRY);
            registerState(SharedStates::Dry(), DRY, PRESERVE);
            registerState(SharedStates::Preserve(), PRESERVE, STOP);
            registerState(Stop(), STOP, IDLE);
            registerState(Idle(), IDLE);
        }

        void begin() override {
            transitionTo(FLUSH);
        }

        void stop() override {
            transitionTo(STOP);
        }

        void idle() override {
            transitionTo(IDLE);
        }
    };
};  // namespace Main

using MainStateController = Main::Controller;

#pragma once
#include <Components/StateController.hpp>
#include <States/Shared.hpp>

namespace Debubble {
    STATE(IDLE);
    STATE(STOP);
    STATE(ALCOHOL_PURGE);

    struct Config {
        decltype(SharedStates::AlcoholPurge::time) time;
    };

    class Controller : public StateControllerWithConfig<Config> {
    public:
        Controller() : StateControllerWithConfig("debubble-state-machine") {}

        // FLUSH -> OFFSHOOT_PRELOAD -> STOP -> IDLE
        void setup() override {
            registerState(SharedStates::AlcoholPurge(), ALCOHOL_PURGE, STOP);
            registerState(SharedStates::Stop(), STOP, IDLE);
            registerState(SharedStates::Idle(), IDLE);
        }

        void begin() override {
            decltype(auto) alcohol_purge = getState<SharedStates::AlcoholPurge>(ALCOHOL_PURGE);
            alcohol_purge.time           = config.time;

            transitionTo(ALCOHOL_PURGE);
        }

        void stop() override {
            transitionTo(STOP);
        }

        void idle() override {
            transitionTo(IDLE);
        }
    };
}  // namespace Debubble

using DebubbleStateController = Debubble::Controller;

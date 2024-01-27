#pragma once
#include <Components/StateController.hpp>
#include <States/Shared.hpp>
#include <StateControllers/MainStateController.hpp>

namespace New {
    STATE(IDLE);
    STATE(FLUSH_1);
    STATE(OFFSHOOT_CLEAN_1);
    STATE(FLUSH_2);
    STATE(SAMPLE);
    STATE(FLUSH_3);
    STATE(OFFSHOOT_CLEAN_2);
    STATE(DEPRESSURE);
    STATE(PRESERVE_FLUSH);
    STATE(PRESERVE);
    STATE(AIR_FLUSH);
    STATE(STOP);

    struct Config {
        decltype(SharedStates::Flush::time) flushTime;
        decltype(SharedStates::Sample::time) sampleTime;
        decltype(SharedStates::Sample::pressure) samplePressure;
        decltype(SharedStates::Sample::volume) sampleVolume;
        decltype(SharedStates::Preserve::time) preserveTime;
    };

    class Controller : public StateController, public StateControllerConfig<Config> {
    public:
        Controller() : StateController("new-state-controller") {}

        void setup();
        void configureStates() {
            decltype(auto) flush1 = getState<SharedStates::Flush>(FLUSH_1);
            flush1.time           = config.flushTime;

            decltype(auto) flush2 = getState<SharedStates::Flush>(FLUSH_2);
            flush2.time           = config.flushTime;
            
            decltype(auto) flush3 = getState<SharedStates::Flush>(FLUSH_3);
            flush3.time           = config.flushTime;
            decltype(auto) sample = getState<SharedStates::Sample>(SAMPLE);
            sample.time           = config.sampleTime;
            sample.pressure       = config.samplePressure;
            sample.volume         = config.sampleVolume;

            decltype(auto) preserve = getState<SharedStates::Preserve>(PRESERVE);
            preserve.time           = config.preserveTime;
        }

        void begin() override {
            configureStates();
            transitionTo(FLUSH_1);
        }

        void stop() override {
            transitionTo(STOP);
        }

        void idle() override {
            transitionTo(IDLE);
        }

        bool isStop() {
            return getCurrentState()->getName() == STOP;
        }
    };
};  // namespace New

using NewStateController = New::Controller;

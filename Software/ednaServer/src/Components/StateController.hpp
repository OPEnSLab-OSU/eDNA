#pragma once
#include <KPFoundation.hpp>
#include <KPStateMachine.hpp>
#include <KPState.hpp>
#include <type_traits>

#define STATE(x) constexpr const char * x = #x "_STATE"

template <typename T>
struct StateControllerConfig {
    using Config       = T;
    using Configurator = struct StateControllerConfigurator {
        using Config                                   = T;
        virtual void operator()(Config & config) const = 0;
    };

    Config config;

    template <typename U>
    void configure(U && configurator) {
        std::forward<U>(configurator)(config);
    }

    // This allows passing of config struct
    void configure(const Config && config) {
        this->config = std::forward<Config>(config);
    }
};

class StateController : public KPStateMachine {
private:
    virtual void begin() = 0;
    virtual void stop()  = 0;
    virtual void idle()  = 0;

public:
    StateController(const char * name) : KPStateMachine(name) {}

    virtual void setup() override {
        println("StateMachine Setup ");
    }
};

template <typename T>
class StateControllerWithConfig : public StateController, public StateControllerConfig<T> {
public:
    using StateController::StateController;
    // using Configurator = StateControllerConfigurator<T>;
};
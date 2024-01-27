#pragma once
#define ARDUINOJSON_USE_LONG_LONG 1
#include <KPFoundation.hpp>
#include <ArduinoJson.h>
#include <tuple>
#include <Application/Status.hpp>

template <typename Signature>
struct APISpec;

template <typename _R, typename... Args>
struct APISpec<_R(Args...)> {
    using R         = _R;
    using ArgsTuple = std::tuple<Args...>;

    template <size_t I>
    using Arg = typename std::tuple_element<I, ArgsTuple>::type;
};

class App;
namespace API {
    template <size_t size>
    using JsonResponse = StaticJsonDocument<size>;

    struct StartHyperFlush : APISpec<JsonResponse<300>(App &)> {
        auto operator()(Arg<0>) -> R;
    };

    struct StartNowTask : APISpec<JsonResponse<300>(App &)> {
        auto operator()(Arg<0>) -> R;
    };
    
    struct StartDebubble : APISpec<JsonResponse<300>(App &)> {
        auto operator()(Arg<0>) -> R;
    };

    struct StatusGet : APISpec<JsonResponse<Status::encodingSize()>(App &)> {
        auto operator()(Arg<0>) -> R;
    };

    struct ConfigGet : APISpec<JsonResponse<Config::encodingSize()>(App &)> {
        auto operator()(Arg<0>) -> R;
    };

    struct TaskCreate : APISpec<JsonResponse<Task::encodingSize() + 500>(App &, JsonDocument &)> {
        auto operator()(Arg<0>, Arg<1>) -> R;
    };

    struct TaskGet : APISpec<JsonResponse<Task::encodingSize() + 500>(App &, JsonDocument &)> {
        auto operator()(Arg<0>, Arg<1>) -> R;
    };

    struct TaskSave : APISpec<JsonResponse<Task::encodingSize() + 500>(App &, JsonDocument &)> {
        auto operator()(Arg<0>, Arg<1>) -> R;
    };

    struct NowTaskSave : APISpec<JsonResponse<NowTask::encodingSize() + 500>(App &, JsonDocument &)> {
        auto operator()(Arg<0>, Arg<1>) -> R;
    };

    struct TaskDelete : APISpec<JsonResponse<500>(App &, JsonDocument &)> {
        auto operator()(Arg<0>, Arg<1>) -> R;
    };

    struct TaskSchedule : APISpec<JsonResponse<Task::encodingSize() + 500>(App &, JsonDocument &)> {
        auto operator()(Arg<0>, Arg<1>) -> R;
    };

    struct TaskUnschedule
        : APISpec<JsonResponse<Task::encodingSize() + 500>(App &, JsonDocument &)> {
        auto operator()(Arg<0>, Arg<1>) -> R;
    };

    struct RTCUpdate : APISpec<JsonResponse<100>(App &, JsonDocument &)> {
        auto operator()(Arg<0>, Arg<1>) -> R;
    };

    struct PressureUpdate : APISpec<JsonResponse<100>(App &, JsonDocument &)> {
        auto operator()(Arg<0>, Arg<1>) -> R;
    };
};  // namespace API

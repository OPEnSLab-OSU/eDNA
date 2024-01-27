#pragma once

class ScheduleReturnCode {
public:
    enum Code { unavailable, operating, scheduled } _code;

public:
    ScheduleReturnCode(Code code) : _code(code) {}

    Code code() const {
        return _code;
    }

    const char * description() const {
        switch (_code) {
        case unavailable:
            return "No active task";
        case operating:
            return "Task is being executed";
        case scheduled:
            return "Task scheduled for execution";
        default:
            halt(TRACE, "Unknown case");
        }
    }

    const char * c_str() const {
        switch (_code) {
        case unavailable:
            return "unavailable";
        case operating:
            return "operating";
        case scheduled:
            return "scheduled";
        default:
            halt(TRACE, "Unknown case");
        }
    }

    // Implicit conversion to int
    operator int() const {
        return _code;
    }
};
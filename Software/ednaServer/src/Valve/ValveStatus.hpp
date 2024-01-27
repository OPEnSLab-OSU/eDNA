#pragma once

//
// ──────────────────────────────────────────────────────────────── I ──────────
//   :::::: V A L V E   S T A T U S : :  :   :    :     :        :          :
// ──────────────────────────────────────────────────────────────────────────
//
// This pattern provides the same scoping functionality as class enum but is
// actually a class and therefore much more powerful
//
class ValveStatus {
public:
    enum Code {
        unavailable = -1,
        sampled,    // 0
        free,       // 1
        operating,  // 2
    } _code;

    ValveStatus(Code code) : _code(code) {}

    Code code() const {
        return _code;
    }

    // Implicit conversion to int
    operator int() const {
        return _code;
    }
};
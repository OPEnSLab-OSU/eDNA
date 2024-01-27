#pragma once

//
// ──────────────────────────────────────────────────────────────────────── I ──────────
//   :::::: T A S K   S T A T U S   E N U M : :  :   :    :     :        :          :
// ──────────────────────────────────────────────────────────────────────────────────
//
// This pattern provides the same scoping functionality as class enum but is a class and
// therefore much more powerful
//
class TaskStatus {
public:
    enum Code {
        inactive = 0,
        active,     // 1
        completed,  // 2
        missed      // 3
    } _code;

    TaskStatus(Code code) : _code(code) {}

    Code code() const {
        return _code;
    }

    // Implicit conversion to int
    operator int() const {
        return _code;
    }
};
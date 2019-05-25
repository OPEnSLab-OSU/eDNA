#pragma once
#include "Foundation.hpp" 

// This class is not intended to be subclassed
struct Action {
private:
    int repeat = 1;
    unsigned long start = 0;
    unsigned long delay = 0;
    VoidFunction callback = nullptr;

    Action * next = nullptr; 

    friend class ActionScheduler;

    void run() {
        callback();
        repeat = max(-1, repeat - 1);
    }
    
public:
    Action() {}
    
    Action(const Action & rhs) {
        repeat = rhs.repeat;
        start = rhs.start;
        delay = rhs.delay;
        callback = rhs.callback;
    }
    
    Action & operator=(const Action & rhs) {
        repeat = rhs.repeat;
        start = rhs.start;
        delay = rhs.delay;
        callback = rhs.callback;
        return * this;
    }

    Action & waitFor(unsigned long ms, VoidFunction callback) {
        delay = ms;
        callback = callback;
        return * this;
    }

    Action & repeatFor(int times) {
        repeat = times;
        return * this;
    }

    Action & repeatForever() {
        repeat = -1;
        return * this;
    }
    
    friend bool operator==(const Action & lhs, const Action & rhs) {
        return
        lhs.repeat == rhs.repeat        &&
        lhs.start == rhs.start          &&
        lhs.delay == rhs.delay    		&&
        lhs.callback == rhs.callback;
    }
    
    friend bool operator!=(const Action & lhs, const Action & rhs) {
        return !(lhs == rhs);
    }
};
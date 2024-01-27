#pragma once
#include <KPObserver.hpp>
#include <Valve/Valve.hpp>
#include <vector>

class ValveObserver : public KPObserver {
public:
    const char * ObserverName() const {
        return ValveObserverName();
    }

    virtual const char * ValveObserverName() const {
        return "<Unnamed> valve observer";
    }

    /** ────────────────────────────────────────────────────────────────────────────
     *  @brief Subscribe to an event when many valves are updated at the same time
     *
     *  @param valves Array of Valve objects
     *  ──────────────────────────────────────────────────────────────────────────── */
    virtual void valveArrayDidUpdate(const std::vector<Valve> & valves) = 0;

    /** ────────────────────────────────────────────────────────────────────────────
     *  @brief Subscribe to an event when a valve is updated
     *
     *  @param valve Valve object
     *  ──────────────────────────────────────────────────────────────────────────── */
    virtual void valveDidUpdate(const Valve & valve) = 0;
};

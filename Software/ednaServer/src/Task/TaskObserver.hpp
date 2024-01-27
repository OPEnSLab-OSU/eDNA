#pragma once
#include <KPObserver.hpp>
#include <Task/Task.hpp>
#include <vector>

class TaskObserver : public KPObserver {
public:
    const char * ObserverName() const {
        return TaskObserverName();
    }

    virtual const char * TaskObserverName() const {
        return "<Unnamed> Task Observer";
    }

    virtual void taskDidUpdate(const Task & task) = 0;
    virtual void taskDidDelete(int id)            = 0;
    virtual void taskDidComplete()                = 0;
};
#pragma once
#include <KPObserver.hpp>
#include <Task/NowTask.hpp>
#include <vector>

class NowTaskObserver : public KPObserver {
public:
    const char * ObserverName() const {
        return NowTaskObserverName();
    }

    virtual const char * NowTaskObserverName() const {
        return "<Unnamed> Now Task Observer";
    }

    virtual void nowTaskDidUpdate(const NowTask & task)     = 0;
    virtual void nowTaskDidComplete(const NowTask & task)   = 0;
    //virtual void taskDidDelete(int id)            = 0;
};
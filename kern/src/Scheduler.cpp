/*
 * @file Scheduler.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/Scheduler.hpp"

namespace simph {
	namespace kern {

// --------------------------------------------------------------------
// ..........................................................
class Schedule {
public:
    Schedule(Smp::IEntryPoint* ep, Smp::Duration simTime, Scheduler* owner):
            _ep(ep),
            _simTime(simTime),
            _owner(owner),
            _completed(false) {
        // TODO mutex to ensure correct id sequencing
        _id=_nextId;
        _nextId++;
    }
    inline bool isCompleted() const {
        return _completed;
    }
    void setTime(Smp::Duration simTime) {
        _simTime=simTime;
    }
    inline Smp::Duration getTime() const {
        return _simTime;
    }
    inline Smp::Services::EventId getId() {
        return _id;
    }
    void run() {
        runEp();
        setCompleted();
    }
protected:
    inline void runEp() {
        _ep->Execute();
    }
    inline void setCompleted() {
        _completed=true;
    }
    inline Scheduler* getOwner() const {
        return _owner;
    }
private:
    Smp::IEntryPoint* _ep;
    Smp::Duration _simTime;
    Scheduler* _owner;
    bool _completed;
    Smp::Services::EventId _id;
    static Smp::Services::EventId _nextId;
};
bool scheduleOrder(Schedule* a,Schedule *b) {
    return a->getTime() < b->getTime();
}
// ..........................................................
class CyclicSchedule: public Schedule {
public:
    CyclicSchedule(Smp::IEntryPoint * ep, Smp::Duration simTime,
            Scheduler* owner,
            Smp::Duration period,Smp::Int64 count):
                Schedule(ep,simTime,owner) {
    }
    void run() {
        runEp();
        if (_count <0 || _count>1) {
            setTime(getTime()+_period);
            getOwner()->schedule(this);
        }
        else {
            setCompleted();
        }
    }
private:
    Smp::Duration _period;
    Smp::Int64 _count;
};

// --------------------------------------------------------------------
// ..........................................................
Scheduler::Scheduler(Smp::String8 name, Smp::String8 descr,
                    Smp::IObject* parent): 
                    Component(name,descr,parent),
                    _scheduled(scheduleOrder) {
}
// ..........................................................
Scheduler::~Scheduler() {
    // TODO delete remaing schedule if any...
}


// --------------------------------------------------------------------
// ..........................................................
void Scheduler::schedule(Schedule* schedule) {
    _scheduled.insert(schedule);
}
// ..........................................................
void Scheduler::step() {
    if (!_scheduled.empty()) {
        Schedule* s=*(_scheduled.begin());
        s->run();
        _scheduled.erase(_scheduled.begin());
        if (s->isCompleted()) {
            delete s;
        }
    }
}
}} // namespace simph::kern

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
#include "Smp/ISimulator.h"

namespace simph {
	namespace kern {

// --------------------------------------------------------------------
// ..........................................................
class Schedule {
public:
    Schedule(const Smp::IEntryPoint* ep, Smp::Duration simTime, Scheduler* owner):
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
    const Smp::IEntryPoint* _ep;
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
    CyclicSchedule(const Smp::IEntryPoint * ep, Smp::Duration simTime,
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
void Scheduler::connect() {
    _timeKeeper=getSimulator()->GetTimeKeeper();
}
// --------------------------------------------------------------------
// ..........................................................
Smp::Services::EventId Scheduler::AddImmediateEvent(const Smp::IEntryPoint* entryPoint) {
    Schedule* s=new Schedule(entryPoint,_timeKeeper->GetSimulationTime(),this);
    _scheduled.insert(s);
    return s->getId();
}
// ..........................................................
Smp::Services::EventId Scheduler::schedule(
                                const Smp::IEntryPoint* entryPoint,
                                Smp::Duration absoluteSimTime,
                                Smp::Duration cycleTime,
                                Smp::Int64 repeat) {
    Schedule* s;
    if (repeat==0) {
        s=new Schedule(entryPoint,absoluteSimTime,this);
    }
    else {
        s=new CyclicSchedule(entryPoint,absoluteSimTime,this,cycleTime,repeat);
    }
    _scheduled.insert(s);
    return s->getId();
}
// ..........................................................
Smp::Services::EventId Scheduler::AddSimulationTimeEvent(
                                const Smp::IEntryPoint* entryPoint,
                                Smp::Duration simulationTime,
                                Smp::Duration cycleTime,
                                Smp::Int64 repeat) {
    return schedule(entryPoint,
                    _timeKeeper->GetSimulationTime()+simulationTime,
                    cycleTime,
                    repeat);
};
// ..........................................................
Smp::Services::EventId Scheduler::AddMissionTimeEvent(
                                const Smp::IEntryPoint* entryPoint,
                                Smp::Duration missionTime,
                                Smp::Duration cycleTime,
                                Smp::Int64 repeat) {
    return schedule(entryPoint,
                    _timeKeeper->GetSimulationTime()+missionTime
                            -_timeKeeper->GetMissionTime(),
                    cycleTime,
                    repeat);
}
// ..........................................................
Smp::Services::EventId Scheduler::AddEpochTimeEvent(
                                const Smp::IEntryPoint* entryPoint,
                                Smp::DateTime epochTime,
                                Smp::Duration cycleTime,
                                Smp::Int64 repeat) {
    return schedule(entryPoint,
                    _timeKeeper->GetSimulationTime()+epochTime
                            -_timeKeeper->GetEpochTime(),
                    cycleTime,
                    repeat);
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
        // advance simulation time to event time.
        // TODO maybe only if event time is greater than current timekeeper time.
        // TODO emit the required events....
        _timeKeeper->SetSimulationTime(s->getTime());
        s->run();
        _scheduled.erase(_scheduled.begin());
        if (s->isCompleted()) {
            delete s;
        }
    }
}
}} // namespace simph::kern

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
#include "simph/kern/Logger.hpp"
#include "simph/kern/ObjectsRegistry.hpp"
#include "Smp/IDataflowField.h"

namespace simph {
	namespace kern {

// --------------------------------------------------------------------
// ..........................................................
class Schedule {
public:
    Schedule(const Smp::IEntryPoint* ep, Smp::Duration simTime,
            Scheduler* owner, const std::vector<Smp::IDataflowField*>* fields,
            Smp::Duration period=0,Smp::Int64 repeat=0):
            _ep(ep),
            _simTime(simTime),
            _owner(owner),
            _fields(fields),
            _period(period),
            _repeat(repeat),
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
    inline void setPeriod(Smp::Duration period) {
        _period=period;
    }
    inline void setRepeat(Smp::Int64 repeat) {
        _repeat=repeat;
    }
    inline Smp::Services::EventId getId() {
        return _id;
    }
    void run() {
        _ep->Execute();
        // Push related data flow fields if any.
        if (_fields!=nullptr) {
            for (auto f: *_fields) {
                f->Push();
            }
        }
        if (_repeat!=0) {
            if (_repeat>0) {
                --_repeat;
            }
            if (_period>0) {
                setTime(getTime()+_period);
                _owner->schedule(this);
            }
        }
        else {
            _completed=true;
        }
    }
private:
    const Smp::IEntryPoint* _ep;
    Smp::Duration _simTime;
    Scheduler* _owner;
    const std::vector<Smp::IDataflowField*>* _fields;
    Smp::Duration _period;
    Smp::Int64 _repeat;
    bool _completed;
    Smp::Services::EventId _id;
    static Smp::Services::EventId _nextId;
};
// ..........................................................
Smp::Services::EventId Schedule::_nextId=0;
// ..........................................................
bool scheduleOrder(Schedule* a,Schedule *b) {
    return a->getTime() < b->getTime();
}

// --------------------------------------------------------------------
// ..........................................................
Scheduler::Scheduler(Smp::String8 name, Smp::String8 descr,
                    Smp::IObject* parent): 
                    Component(name,descr,parent),
                    _scheduled(scheduleOrder) {
}
// ..........................................................
Scheduler::~Scheduler() {
    // TODO delete remaining schedule if any...
}
// --------------------------------------------------------------------
// ..........................................................
void Scheduler::connect() {
    _timeKeeper=getSimulator()->GetTimeKeeper();
}
// --------------------------------------------------------------------
// ..........................................................
Smp::Services::EventId Scheduler::AddImmediateEvent(const Smp::IEntryPoint* entryPoint) {

    return schedule(entryPoint,_timeKeeper->GetSimulationTime(),0,0);
}
// ..........................................................
Smp::Services::EventId Scheduler::schedule(
                                const Smp::IEntryPoint* entryPoint,
                                Smp::Duration absoluteSimTime,
                                Smp::Duration cycleTime,
                                Smp::Int64 repeat) {
    const std::vector<Smp::IDataflowField*>* flowFields=nullptr;
    auto reg=dynamic_cast<ObjectsRegistry*>(getSimulator()->GetResolver());
    if (reg!=nullptr) {
        flowFields=reg->getRelatedFlowFields(entryPoint);
    }
    Schedule* s=new Schedule(entryPoint,absoluteSimTime,this,
                    flowFields,cycleTime,repeat);
    _scheduled.insert(s);
    return s->getId();
}
// ..........................................................
Schedule* Scheduler::findSchedule(Smp::Services::EventId event,
                                    bool erase) {
    Schedule* res=nullptr;
    auto it=_scheduled.begin();
    while (it!=_scheduled.end() && (*it)->getId()!=event) {
        ++it;
    }
    if (it!=_scheduled.end()) {
        res=*it;
        if (erase) {
            _scheduled.erase(it);
        }
    }
    return res;
}
// ..........................................................
void Scheduler::schedule(Smp::Services::EventId event,
                         Smp::Duration absoluteSimTime) {
    Schedule* s=findSchedule(event,true);
    if (s!=nullptr) {
        s->setTime(absoluteSimTime);
        _scheduled.insert(s);
    }
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
// ..........................................................
Smp::Services::EventId Scheduler::AddZuluTimeEvent(
                                const Smp::IEntryPoint* entryPoint,
                                Smp::DateTime zuluTime,
                                Smp::Duration cycleTime,
                                Smp::Int64 repeat) {
    LOGE("Scheduler::AddZuluTimeEvent not implemented!!! (unsure of what shall be really done and what are the use case.")
    return -1;
}
// ..........................................................
void Scheduler::SetEventSimulationTime(Smp::Services::EventId event,
                                       Smp::Duration simulationTime) {
    schedule(event,simulationTime+_timeKeeper->GetSimulationTime());
}
// ..........................................................
void Scheduler::SetEventMissionTime(Smp::Services::EventId event,
                                       Smp::Duration missionTime) {
    schedule(event,_timeKeeper->GetSimulationTime()+missionTime
                            -_timeKeeper->GetMissionTime());
}
// ..........................................................
void Scheduler::SetEventEpochTime(Smp::Services::EventId event,
                                       Smp::DateTime epochTime) {
    schedule(event,_timeKeeper->GetSimulationTime()+epochTime
                            -_timeKeeper->GetEpochTime());
}
// ..........................................................
void Scheduler::SetEventZuluTime(Smp::Services::EventId event,
                                       Smp::DateTime zuluTime) {
    LOGE("Scheduler::SetEventZuluTime not implemented!!! (unsure of what shall be really done and what are the use case.")
}
// ..........................................................
void Scheduler::SetEventCycleTime(Smp::Services::EventId event,
                                Smp::Duration cycleTime) {
    Schedule* s=findSchedule(event);
    if (s!=nullptr) {
        s->setPeriod(cycleTime);
    }
}
// ..........................................................
void Scheduler::SetEventRepeat(Smp::Services::EventId event,
                            Smp::Int64 repeat) {
    Schedule* s=findSchedule(event);
    if (s!=nullptr) {
        s->setRepeat(repeat);
    }
}
// ..........................................................
void Scheduler::RemoveEvent(Smp::Services::EventId event) {
    findSchedule(event,true);
}
// ..........................................................
Smp::Services::EventId Scheduler::GetCurrentEventId() const {
    // TODO mutex...
    if (_currentSchedule!=nullptr) {
        return _currentSchedule->getId();
    }
    return -1;
}
// ..........................................................
Smp::Duration Scheduler::GetNextScheduledEventTime() const {
    if (!_scheduled.empty()) {
        Schedule* s=*(_scheduled.begin());
        return s->getTime();
    }
    return 0;
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

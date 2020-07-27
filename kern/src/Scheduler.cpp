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
#include "simph/kern/TimeKeeper.hpp"
#include "Smp/IDataflowField.h"
#include "assert.h"
#include <atomic>

namespace simph {
	namespace kern {

// --------------------------------------------------------------------
// ..........................................................
class Scheduler::Schedule {
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
        static std::atomic<Smp::Services::EventId> _nextId(0);
        _id = _nextId++;
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
    inline Smp::Services::EventId getId() const {
        return _id;
    }
    inline bool isCompleted() const {
        return _completed;
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
                _repeat -= 1;
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
    Smp::Services::EventId _id;
    bool _completed;
};
// --------------------------------------------------------------------
// ..........................................................
Scheduler::Scheduler(Smp::String8 name, Smp::String8 descr,
                    Smp::IObject* parent): 
                    Component(name,descr,parent),
                    _timeKeeper(nullptr),
                    _autoStop(false),
                    _stopSimTime(0),
                    _run(false),
                    _mutex(),
                    _th(),
                    _currentSchedule(nullptr),
                    _scheduled(compareSchedule) {
}
// ..........................................................
Scheduler::~Scheduler() {
    // Ensure it is no more running
    stop();
    // delete all remaining schedules.
    for (auto s: _scheduled) {
        delete s;
    }
}
// --------------------------------------------------------------------
// ..........................................................
bool Scheduler::compareSchedule(const Schedule* a, const Schedule* b) {
    // To define order:
    // - 1st check time (obviously)
    // - then if equals, check insert order (id is used since id is created and
    // incremented on 1st insertion.
    Smp::Duration ta=a->getTime();
    Smp::Duration tb=b->getTime();
    return ta < tb || (ta==tb && a->getId()<b->getId());
}
// ..........................................................
void Scheduler::schedule(Schedule* s) {
    Synchronized(_mutex)
    _scheduled.insert(s);
}
// ..........................................................
void Scheduler::connect() {
    _timeKeeper=dynamic_cast<TimeKeeper*>(getSimulator()->GetTimeKeeper());
}
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
    TRACE("[Scheduler::schedule] entryPoint='"<<entryPoint->GetName()<<"',absoluteSimTime="<<absoluteSimTime<<",cycleTime="<<cycleTime<<",repeat="<<repeat);
    const std::vector<Smp::IDataflowField*>* flowFields=nullptr;
    auto reg=dynamic_cast<ObjectsRegistry*>(getSimulator()->GetResolver());
    if (reg!=nullptr) {
        flowFields=reg->getRelatedFlowFields(entryPoint);
    }

    auto mySchedule = new Schedule( entryPoint,
                                                  absoluteSimTime,
                                                  this,
                                                  flowFields,
                                                  cycleTime,
                                                  repeat);
    schedule(mySchedule);
    return mySchedule->getId();
}
// ..........................................................
Scheduler::Schedule* Scheduler::findSchedule(Smp::Services::EventId event,bool remove) {
    Schedule* res=nullptr;
    Synchronized(_mutex);
    if (_currentSchedule!=nullptr && _currentSchedule->getId()==event) {
        res=_currentSchedule;
    }
    else {
        for (auto it=_scheduled.begin();res==nullptr && it!=_scheduled.end();++it) {
            if ((*it)->getId()==event) {
                res=*it;
                if (remove) {
                    _scheduled.erase(it);
                }
            }
        }
    }
    return res;
}
// ..........................................................
void Scheduler::schedule(Smp::Services::EventId event,
                         Smp::Duration absoluteSimTime) {
    // TODO think about it is valid to do that from the
    // scheduled entry point itself. Or what does it means if an entry
    // point change its own simulation time (and only simulation time if
    // scheduled once (with no repeat).
    auto s=findSchedule(event,true);
    if (s) {
        s->setTime(absoluteSimTime);
        schedule(s);
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
    Synchronized(_mutex);
    auto s=findSchedule(event);
    if (s) {
        s->setPeriod(cycleTime);
    }
}
// ..........................................................
void Scheduler::SetEventRepeat(Smp::Services::EventId event,
                            Smp::Int64 repeat) {
    Synchronized(_mutex);
    auto s=findSchedule(event);
    if (s) {
        s->setRepeat(repeat);
    }
}
// ..........................................................
void Scheduler::RemoveEvent(Smp::Services::EventId event) {
    Synchronized(_mutex);
    Schedule* s=findSchedule(event,true);
    if (s!=nullptr && s!=_currentSchedule) {
        delete s;
    }
}
// ..........................................................
Smp::Services::EventId Scheduler::GetCurrentEventId() const {
    Synchronized(_mutex);
    if (_currentSchedule) {
        return _currentSchedule->getId();
    }
    return -1;
}
// ..........................................................
Smp::Duration Scheduler::GetNextScheduledEventTime() const {
    Synchronized(_mutex);
    if (!_scheduled.empty()) {
        return (*_scheduled.begin())->getTime();
    }
    return 0;
}
// ..........................................................
void Scheduler::step() {
    Schedule* toRun=nullptr;
    {
        Synchronized(_mutex)
        if (!_scheduled.empty()) {
            auto top=_scheduled.begin();
            _currentSchedule=*top;
            _scheduled.erase(top);
            toRun=_currentSchedule;
        }
    }
    if (toRun!=nullptr) {
        // advance simulation time to event time.
        // Exception and event handling is done by time keeper itself.
        _timeKeeper->setNextEventTime(toRun->getTime());
        toRun->run();
        Synchronized(_mutex)
        if (toRun->isCompleted()) {
            delete toRun;
        }
        if (toRun!=_currentSchedule) {
            logWarning("Scheduler state changed while running an entry point. "
                    " Next scheduling may be corrupted.");
        }
        _currentSchedule=nullptr;
    }
}
// ..........................................................
void Scheduler::step(Smp::Duration duration) {
    _stopSimTime=_timeKeeper->GetSimulationTime()+duration;
    _autoStop=true;
    run();
    _autoStop=false;
}
// ..........................................................
void Scheduler::start() {
    Synchronized(_mutex);
    if (!_run) {
        _th.reset(new simph::sys::Thread(GetName(),this));
        _th->start();
    }
}
// ..........................................................
void Scheduler::stop() {
    _run=false;
    if (_th!=nullptr) {
        _th->join();
        _th.reset();
    }
}
// ..........................................................
void Scheduler::run() {
    _run=true;
    while (_run) {
        step();
        if (_autoStop) {
            Synchronized(_mutex);
            _run&=!_scheduled.empty() &&
                    (_stopSimTime==0 || 
                        (*_scheduled.begin())->getTime()<=_stopSimTime);
        }
    }
}
}} // namespace simph::kern

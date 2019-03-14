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
            _repeat(repeat) {
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
                _repeat -= 1;
            }
            if (_period>0) {
                _owner->schedule(_id,getTime()+_period);
            }
        }
        else {
            // completed
            _owner->RemoveEvent(_id);
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
                    _scheduled(),
                    _scheduledQueue(compareSchedule){
    _scheduled.reserve(1000);
}
// ..........................................................
Scheduler::~Scheduler() {
}
// --------------------------------------------------------------------
bool Scheduler::compareSchedule(const ScheduledQueueData& a, const ScheduledQueueData& b) {
    // TODO check queue order in case of equality
    return a.first > b.first;
}
// ..........................................................
void Scheduler::connect() {
    _timeKeeper=getSimulator()->GetTimeKeeper();
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
    const std::vector<Smp::IDataflowField*>* flowFields=nullptr;
    auto reg=dynamic_cast<ObjectsRegistry*>(getSimulator()->GetResolver());
    if (reg!=nullptr) {
        flowFields=reg->getRelatedFlowFields(entryPoint);
    }

    auto mySchedule = std::make_shared<Schedule>( entryPoint,
                                                  absoluteSimTime,
                                                  this,
                                                  flowFields,
                                                  cycleTime,
                                                  repeat);

    Synchronized(_mutex);
    _scheduled[mySchedule->getId()]=mySchedule;
    pushToQueue(mySchedule);
    return mySchedule->getId();
}
// ..........................................................
void Scheduler::pushToQueue(const SchedulePtr& schedule) {
    _scheduledQueue.push( std::make_pair(
                          schedule->getTime(),
                          std::weak_ptr<Schedule>(schedule)));
}
// ..........................................................
Scheduler::SchedulePtr Scheduler::findSchedule(Smp::Services::EventId event) const{
    auto it = _scheduled.find(event);
    return it!=_scheduled.end() ? it->second: nullptr;
}
// ..........................................................
void Scheduler::schedule(Smp::Services::EventId event,
                         Smp::Duration absoluteSimTime) {
    Synchronized(_mutex);
    auto schedule=findSchedule(event);
    if (schedule) {
        schedule->setTime(absoluteSimTime);
        pushToQueue(schedule);
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
    _scheduled.erase(event);
}
// ..........................................................
Smp::Services::EventId Scheduler::GetCurrentEventId() const {
    if (_currentSchedule) {
        return _currentSchedule->getId();
    }
    return -1;
}
// ..........................................................
Smp::Duration Scheduler::GetNextScheduledEventTime() const {
    if (!_scheduled.empty()) {
         Synchronized(_mutex);
        return _scheduledQueue.top().first;
    }
    return 0;
}
// ..........................................................
void Scheduler::step() {
    if (!_scheduledQueue.empty()) {
        Smp::Duration nextScheduleTime;
        {
            Synchronized(_mutex);
            auto& scheduledQueueData = _scheduledQueue.top();
            nextScheduleTime = scheduledQueueData.first;
            _currentSchedule= scheduledQueueData.second.lock();
            _scheduledQueue.pop();

            if (!_currentSchedule || nextScheduleTime!=_currentSchedule->getTime() ) {
                return;
            }
        }

        // advance simulation time to event time.
        // TODO maybe only if event time is greater than current timekeeper time.
        // TODO emit the required events....
        _timeKeeper->SetSimulationTime(nextScheduleTime);

        _currentSchedule->run();
        _currentSchedule.reset();
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
    if (!_th) {
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
            _run&=!_scheduled.empty() &&
                    (_stopSimTime==0 || 
                        GetNextScheduledEventTime()<_stopSimTime);
        }
    }
}
}} // namespace simph::kern

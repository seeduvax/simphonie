/*
 * @file TimeKeeper.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/TimeKeeper.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>
#include "Smp/IPublication.h"
#include "Smp/ISimulator.h"
#include "abs/profiler.h"
#include "simph/kern/ExInvalidSimulationTime.hpp"
#include "simph/sys/Logger.hpp"

#define EP_NAME_UPDATE_SIM_TIME "updateSimTime"

// only shared contract with the scheduler, 
//   what event to change simulation time
//   what tim value meaning no more event
#define EV_NAME_PRE_EVENT_EXECUTE "Scheduler_PreEventExecute"
#define DURATION_MAX INT64_MAX

namespace simph {
namespace kern {
// ..........................................................
Smp::DateTime __GetY2KOffset() {
    std::tm tm = {};
    std::stringstream ss("2000-01-01 00:00:00+0000");
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S%z");
    auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    return std::chrono::duration_cast<std::chrono::nanoseconds>(tp.time_since_epoch()).count();
}
Smp::DateTime TimeKeeper::_y2kJan1Offset = __GetY2KOffset();
// --------------------------------------------------------------------
// ..........................................................
TimeKeeper::TimeKeeper(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : Component(name, descr, parent) {
    addEP(EP_NAME_UPDATE_SIM_TIME,"simulation time update entry point", 
                                &TimeKeeper::epUpdateSimulationTime,this);
}
// ..........................................................
TimeKeeper::~TimeKeeper() {}
// --------------------------------------------------------------------
// ..........................................................
Smp::Duration TimeKeeper::GetSimulationTime() const {
    return _simTime;
}
// ..........................................................
Smp::DateTime TimeKeeper::GetEpochTime() const {
    return _epochOffset + _simTime;
}
// ..........................................................
Smp::DateTime TimeKeeper::GetMissionStartTime() const {
    return _missionStart;
}
// ..........................................................
Smp::DateTime TimeKeeper::GetMissionTime() const {
    return GetEpochTime() - _missionStart;
}
// ..........................................................
Smp::DateTime TimeKeeper::GetZuluTime() const {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch())
               .count()
           - _y2kJan1Offset;
}
// --------------------------------------------------------------------
// ..........................................................
void TimeKeeper::publish(Smp::IPublication* receiver) {
    receiver->PublishField("simTime", "Simulation time", &_simTime, Smp::ViewKind::VK_All, true, false, true);
}
// --------------------------------------------------------------------
// ..........................................................
void TimeKeeper::epUpdateSimulationTime() {
    auto simulationTime=_scheduler->GetNextScheduledEventTime();
    if (simulationTime > _simTime && simulationTime != DURATION_MAX) {
        _nextEventTime = simulationTime;
        if (_eventMgr != nullptr) {
            _inPreSimTimeChange=true;
            _eventMgr->Emit(Smp::Services::IEventManager::SMP_PreSimTimeChangeId);
            _inPreSimTimeChange=false;
        }
        _simTime = _nextEventTime;
        PROFILER_FRAME("simTime");
        PROFILER_PLOT("simTime", _simTime);
        if (_eventMgr != nullptr) {
            _eventMgr->Emit(Smp::Services::IEventManager::SMP_PostSimTimeChangeId);
        }
        _nextEventTime = 0;
    }
    else if (simulationTime < _simTime) {
        throw ExInvalidSimulationTime(this, _simTime, simulationTime, _nextEventTime);
    }
    // else simulation time is not changed and there is nothing to do.
}
// ..........................................................
void TimeKeeper::SetSimulationTime(Smp::Duration simulationTime) {
    if (_inPreSimTimeChange) {
        if (simulationTime >= _simTime && simulationTime <= _nextEventTime) {
            _simTime = simulationTime;
        }
        else {
            throw ExInvalidSimulationTime(this, _simTime, simulationTime, _nextEventTime);
        }
    }
}
// ..........................................................
void TimeKeeper::SetEpochTime(Smp::DateTime epochTime) {
    _epochOffset = epochTime - _simTime;
    if (_eventMgr != nullptr) {
        _eventMgr->Emit(Smp::Services::IEventManager::SMP_EpochTimeChangedId);
    }
}
// ..........................................................
void TimeKeeper::SetMissionStartTime(Smp::DateTime missionStart) {
    _missionStart = missionStart;
    if (_eventMgr != nullptr) {
        _eventMgr->Emit(Smp::Services::IEventManager::SMP_MissionTimeChangedId);
    }
}
// ..........................................................
void TimeKeeper::SetMissionTime(Smp::Duration missionTime) {
    _missionStart = GetEpochTime() - missionTime;
    if (_eventMgr != nullptr) {
        _eventMgr->Emit(Smp::Services::IEventManager::SMP_MissionTimeChangedId);
    }
}
// --------------------------------------------------------------------
// ..........................................................
void TimeKeeper::connect() {
    _scheduler = getSimulator()->GetScheduler();
    _eventMgr = getSimulator()->GetEventManager();
    auto preEventExecuteId = _eventMgr->QueryEventId(EV_NAME_PRE_EVENT_EXECUTE);
    _eventMgr->Subscribe(preEventExecuteId,GetEntryPoint(EP_NAME_UPDATE_SIM_TIME));
}
// ..........................................................
void TimeKeeper::reset() {
    // default : set epoch time to current zulu time.
    // set simTime to 0
    // set missionTime to 0 too.
    if (_eventMgr != nullptr) {
        _inPreSimTimeChange=true;
        _eventMgr->Emit(Smp::Services::IEventManager::SMP_PreSimTimeChangeId);
        _inPreSimTimeChange=false;
    }
    _simTime = 0;
    _epochOffset = GetZuluTime();
    _missionStart = _epochOffset;
    if (_eventMgr != nullptr) {
        _eventMgr->Emit(Smp::Services::IEventManager::SMP_PostSimTimeChangeId);
        _eventMgr->Emit(Smp::Services::IEventManager::SMP_EpochTimeChangedId);
        _eventMgr->Emit(Smp::Services::IEventManager::SMP_MissionTimeChangedId);
    }
}

}  // namespace kern
}  // namespace simph

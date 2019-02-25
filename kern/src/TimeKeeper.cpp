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
#include "Smp/ISimulator.h"
#include <chrono>
#include <sstream>
#include <iomanip>

namespace Smp {
constexpr Smp::Char8 Smp::Services::ITimeKeeper::SMP_TimeKeeper[];
}

namespace simph {
	namespace kern {
// ..........................................................
Smp::DateTime __GetY2KOffset() {
    std::tm tm = {};
    std::stringstream ss("2000-01-01 00:00:00+0000");
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S%z");
    auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
                tp.time_since_epoch()).count();
    
}
Smp::DateTime TimeKeeper::_y2kJan1Offset=__GetY2KOffset();
// --------------------------------------------------------------------
// ..........................................................
TimeKeeper::TimeKeeper(Smp::String8 name,
                        Smp::String8 descr,
                        Smp::IObject* parent):
                Component(name,descr,parent),
                _eventMgr(nullptr) {
}
// ..........................................................
TimeKeeper::~TimeKeeper() {
}
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
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count()
            - _y2kJan1Offset;
    
}
// --------------------------------------------------------------------
// ..........................................................
void TimeKeeper::SetSimulationTime(Smp::Duration simulationTime) {
    if (simulationTime>_simTime) {
        if (_eventMgr!=nullptr) {
            _eventMgr->Emit(Smp::Services::IEventManager::SMP_PreSimTimeChangeId);
        }
        _simTime=simulationTime;
        if (_eventMgr!=nullptr) {
            _eventMgr->Emit(Smp::Services::IEventManager::SMP_PostSimTimeChangeId);
        }
    }
    else {
// TODO throw exception TBD.
    }
}
// ..........................................................
void TimeKeeper::SetEpochTime(Smp::DateTime epochTime) {
    _epochOffset=epochTime-_simTime;
    if (_eventMgr!=nullptr) {
        _eventMgr->Emit(Smp::Services::IEventManager::SMP_EpochTimeChangedId);
    }
}
// ..........................................................
void TimeKeeper::SetMissionStartTime(Smp::DateTime missionStart) {
    _missionStart=missionStart;
    if (_eventMgr!=nullptr) {
        _eventMgr->Emit(Smp::Services::IEventManager::SMP_MissionTimeChangedId);
    }
}
// ..........................................................
void TimeKeeper::SetMissionTime(Smp::Duration missionTime) {
    _missionStart=GetEpochTime() - missionTime;
    if (_eventMgr!=nullptr) {
        _eventMgr->Emit(Smp::Services::IEventManager::SMP_MissionTimeChangedId);
    }
}
// --------------------------------------------------------------------
// ..........................................................
void TimeKeeper::connect() {
    _eventMgr=getSimulator()->GetEventManager();
}
// ..........................................................
void TimeKeeper::reset() {
    // default : set epoch time to current zulu time.
    // set simTime to 0
    // set missionTime to 0 too.
    if (_eventMgr!=nullptr) {
        _eventMgr->Emit(Smp::Services::IEventManager::SMP_PreSimTimeChangeId);
    }
    _simTime=0;
    _epochOffset=GetZuluTime();
    _missionStart=_epochOffset;
    if (_eventMgr!=nullptr) {
        _eventMgr->Emit(Smp::Services::IEventManager::SMP_PostSimTimeChangeId);
        _eventMgr->Emit(Smp::Services::IEventManager::SMP_EpochTimeChangedId);
        _eventMgr->Emit(Smp::Services::IEventManager::SMP_MissionTimeChangedId);
    }
}

}} // namespace simph::kern

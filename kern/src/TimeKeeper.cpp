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

namespace Smp {
constexpr Smp::Char8 Smp::Services::ITimeKeeper::SMP_TimeKeeper[];
}

namespace simph {
	namespace kern {

Smp::DateTime TimeKeeper::_y2kJan1Offset=100000000;
// --------------------------------------------------------------------
// ..........................................................
TimeKeeper::TimeKeeper(Smp::String8 name,
                        Smp::String8 descr,
                        Smp::IObject* parent):
                Component(name,descr,parent) {
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
    return _epochOffset + _simTime - _missionStart;
}
// ..........................................................
Smp::DateTime TimeKeeper::GetZuluTime() const {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count()
            - _y2kJan1Offset;
    
}

}} // namespace simph::kern

/*
 * @file SysTimeSynchro.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/umdl/SysTimeSynchro.hpp"
#include "Smp/IPublication.h"
#include "abs/profiler.h"

static const std::chrono::time_point<std::chrono::system_clock> _nullTimePoint;

namespace simph {
namespace umdl {
// --------------------------------------------------------------------
// ..........................................................
SysTimeSynchro::SysTimeSynchro(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : AStepMdl(name, descr, parent), _nextTick(_nullTimePoint) {
    _period = 10000000;  // default 100Hz sync.
}
// ..........................................................
SysTimeSynchro::~SysTimeSynchro() {}
// --------------------------------------------------------------------
// ..........................................................
void SysTimeSynchro::publish(Smp::IPublication* receiver) {
    receiver->PublishField("period", "Synchronization period.", &_period, Smp::ViewKind::VK_All, false, true, false);
}
// --------------------------------------------------------------------
// ..........................................................
void SysTimeSynchro::step() {
    if (_nextTick == _nullTimePoint) {
        _nextTick =
            // 1st step:
            // next time to wait unit is now + period
            std::chrono::system_clock::now() + std::chrono::microseconds(_period / 1000);
        // TODO check why nanosec not compiling.
        ;  //+std::chrono::nanoseconds(_period);
    }
    else {
        PROFILER_REGION_COL("SysTimeSynchro", Grey);
        // Wait here until tick time is elapsed.
        Synchronized(_mutex);
        MonitorWaitUntil(_monitor, _nextTick);
        // update nextTock to next period end time
        _nextTick += std::chrono::microseconds(_period / 1000);
    }
}
}  // namespace umdl
}  // namespace simph

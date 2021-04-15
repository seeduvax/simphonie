/*
 * @file SimTimeProvider.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/umdl/SimTimeProvider.hpp"
#include "Smp/ISimulator.h"
#include "Smp/Services/ILogger.h"
#include "Smp/Services/ITimeKeeper.h"
#include "simph/sys/Logger.hpp"

namespace simph {
namespace umdl {
// --------------------------------------------------------------------
// ..........................................................
SimTimeProvider::SimTimeProvider(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : Generator1D(name, descr, parent) {}
// ..........................................................
SimTimeProvider::~SimTimeProvider() {}
// --------------------------------------------------------------------
// ..........................................................
Smp::Float64 SimTimeProvider::compute() {
    Smp::Duration simTime = getSimulator()->GetTimeKeeper()->GetSimulationTime();
    Smp::Float64 res = 0.0;
    res += simTime;
    res /= 1e9;  // convert from nanosec to sec.
#ifdef _abs_trace_debug
    std::ostringstream msg;
    msg << "step t=" << res;
    logDebug(msg.str().c_str());
#endif
    return res;
}

}  // namespace umdl
}  // namespace simph

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
#include "Smp/Services/ITimeKeeper.h"
#include "Smp/Services/ILogger.h"
#include "simph/sys/Logger.hpp"

namespace simph {
	namespace umdl {
// --------------------------------------------------------------------
// ..........................................................
SimTimeProvider::SimTimeProvider(Smp::String8 name, Smp::String8 descr,
                                Smp::IObject* parent):
                    Component(name,descr,parent), 
                    AStepMdl(name,descr,parent), 
                    Generator1D(name,descr,parent) {
}
// ..........................................................
SimTimeProvider::~SimTimeProvider() {
}
// --------------------------------------------------------------------
// ..........................................................
Smp::Float64 SimTimeProvider::compute() {
    Smp::Duration simTime=getSimulator()->GetTimeKeeper()->GetSimulationTime();
    Smp::Float64 res=0.0;
    res+=simTime;
    res/=1e9; // convert from nanosec to sec.
std::ostringstream msg;
msg << "step t="<<res;
getSimulator()->GetLogger()->Log(this,msg.str().c_str());    
    return res;
}

}} // namespace simph::umdl

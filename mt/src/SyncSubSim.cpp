/*
 * @file SyncSubSim.cpp
 *
 * Copyright 2022 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/mt/SyncSubSim.hpp"
#include "Smp/IPublication.h"
#include "simph/kern/Simulator.hpp"
#include "simph/kern/EntryPoint.hpp"
#include "simph/smpdk/Container.hpp"
#include "simph/sys/Callback.hpp"
#include "simph/sys/Logger.hpp"
#include "abs/profiler.h"

namespace simph {
namespace mt {
// --------------------------------------------------------------------
// ..........................................................
SyncSubSim::SyncSubSim(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
        : simph::smpdk::Component(name, descr, parent) {
    _initEP=new simph::kern::EntryPoint(std::move(simph::sys::Callback::create(&SyncSubSim::syncInitEP,this)),"init","",this);
    _syncEP=new simph::kern::EntryPoint(std::move(simph::sys::Callback::create(&SyncSubSim::syncEP,this)),"sync","",this);
    _subSim = new simph::kern::Simulator("sim", "Sub simulator", this);
    _syncRate=10000000; // default 1ms sync rate.
    pthread_barrier_init(&_barrier, nullptr, 2);
    // TODO Simulator is not component. Find another way
    // to contain it, in Smp Composite meaning.
    //    GetContainer("sub")->AddComponent(_subSim);
}
// ..........................................................
SyncSubSim::~SyncSubSim() {
    pthread_barrier_destroy(&_barrier);
    delete _subSim;
    delete _syncEP;
    delete _initEP;
}
// --------------------------------------------------------------------
// ..........................................................
void SyncSubSim::publish(Smp::IPublication* receiver) {
    receiver->PublishField("SyncRate", "Simulators Synchronisation rate",
                           &_syncRate, Smp::ViewKind::VK_All, true, true,
                           false);
    _subSim->Publish();
}
// ..........................................................
void SyncSubSim::configure() {
    _subSim->Configure();
}
// ..........................................................
void SyncSubSim::connect() {
    _subSim->Connect();
    getSimulator()->AddInitEntryPoint(_initEP);
}
// --------------------------------------------------------------------
// ..........................................................
void SyncSubSim::syncEP() {
    PROFILER_FUNCTION
TRACE("")
    pthread_barrier_wait(&_barrier);
}
// ..........................................................
void SyncSubSim::syncInitEP() {
TRACE("")
//    pthread_barrier_init(&_barrier, nullptr, 2);
    _evSyncMaster = getSimulator()->GetScheduler()->AddSimulationTimeEvent(_syncEP, 0, _syncRate,-1);
    _evSyncSub = _subSim->GetScheduler()->AddSimulationTimeEvent(_syncEP, 0, _syncRate,-1);
}
// ..........................................................
void SyncSubSim::syncReleaseEP() {
    pthread_barrier_destroy(&_barrier);
    pthread_barrier_init(&_barrier, nullptr, 0);
    _subSim->Hold(true);
    getSimulator()->GetScheduler()->RemoveEvent(_evSyncMaster);
    _subSim->GetScheduler()->RemoveEvent(_evSyncSub);
}

}  // namespace mt
}  // namespace simph

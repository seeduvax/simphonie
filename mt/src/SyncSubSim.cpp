/*
 * @file SyncSubSim.cpp
 *
 * Copyright 2022 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/mt/SyncSubSim.hpp"
#include "Smp/IPublication.h"
#include "Smp/Services/IEventManager.h"
#include "Smp/Services/IScheduler.h"
#include "simphonie/kern/Simulator.hpp"
#include "simdeck/EntryPoint.hpp"
#include "simdeck/Container.hpp"
#include "simphonie/sys/Callback.hpp"
#include "simphonie/sys/Logger.hpp"
#include "abs/profiler.h"

namespace simphonie {
namespace mt {
// --------------------------------------------------------------------
// ..........................................................
SyncSubSim::SyncSubSim(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
        : simdeck::Component(name, descr, parent) {
    _initEP=simdeck::EntryPoint::Create("sincInit", "Subsimulator isynchronization initialization entry point",
                                            this, &SyncSubSim::epSyncInit);
    _syncEP=simdeck::EntryPoint::Create("sync", "Subsimulator synchronization entry point", 
                                            this, &SyncSubSim::epSync);
    _enterExecutingEP=simdeck::EntryPoint::Create("Run", "Subsimulator run entry point",
                                            this, &SyncSubSim::epRun);
    _leaveExecutingEP=simdeck::EntryPoint::Create("Hold", "Subsimulator hold entry point",
                                            this, &SyncSubSim::epHold);
    _subSim = new simphonie::kern::Simulator(name, "Sub simulator", this);
    _syncRate=1000000; // default 1ms sync rate.
    pthread_barrier_init(&_barrier, nullptr, 0);

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
    delete _enterExecutingEP;
    delete _leaveExecutingEP;
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
    auto evMgr=getSimulator()->GetEventManager();
    evMgr->Subscribe(Smp::Services::IEventManager::SMP_LeaveExecutingId,_leaveExecutingEP);
    evMgr->Subscribe(Smp::Services::IEventManager::SMP_EnterExecutingId,_enterExecutingEP);
}
// --------------------------------------------------------------------
// ..........................................................
void SyncSubSim::epSync() {
    PROFILER_FUNCTION
    pthread_barrier_wait(&_barrier);
}
// ..........................................................
void SyncSubSim::epSyncInit() {
    _evSyncMaster = getSimulator()->GetScheduler()->AddSimulationTimeEvent(_syncEP, 0, _syncRate,-1);
    _evSyncSub = _subSim->GetScheduler()->AddSimulationTimeEvent(_syncEP, 0, _syncRate,-1);
}
// ..........................................................
void SyncSubSim::epRun() {
    pthread_barrier_init(&_barrier, nullptr, 2);
    _subSim->Run();
}
// ..........................................................
void SyncSubSim::epHold() {
    pthread_barrier_init(&_barrier, nullptr, 0);
    // This wait should unlock the other thread, if currently waiting.
    pthread_barrier_wait(&_barrier);
    // From here, slave is unblocked and may run to many things before 
    // receving Hold. May be somthing more shall be done to drain the 
    // schedule queue or freeze somehow the scheduler without blocking th Hold
    // request.
    _subSim->Hold(true);
}
// ..........................................................

}  // namespace mt
}  // namespace simph

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
#include "simph/smpdk/Container.hpp"

namespace simph {
namespace mt {
// --------------------------------------------------------------------
// ..........................................................
SyncSubSim::SyncSubSim(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : simph::smpdk::Component(name, descr, parent), _containers("Containers", "", this) {
    _subSim = new simph::kern::Simulator("sim", "Sub simulator", this);
    _containers.push_back(new simph::smpdk::Container("sub", "", this));
    pthread_barrier_init(&_barrier, nullptr, 0);
    // TODO Simulator is not a component. Find another way
    // to contain it, in Smp Composite meaning.
    //    GetContainer("sub")->AddComponent(_subSim);
}
// ..........................................................
SyncSubSim::~SyncSubSim() {
    delete _subSim;
    pthread_barrier_destroy(&_barrier);
}
// --------------------------------------------------------------------
// ..........................................................
const Smp::ContainerCollection* SyncSubSim::GetContainers() const {
    return &_containers;
}
// ..........................................................
Smp::IContainer* SyncSubSim::GetContainer(Smp::String8 name) const {
    return _containers.at(name);
}

// --------------------------------------------------------------------
// ..........................................................
void SyncSubSim::publish(Smp::IPublication* receiver) {
    receiver->PublishField("SyncRate", "Simulators Synchronisation rate", &_syncRate, Smp::ViewKind::VK_All, true, true,
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
}
// --------------------------------------------------------------------
// ..........................................................
void SyncSubSim::syncEP() {
    pthread_barrier_wait(&_barrier);
}
// ..........................................................
void SyncSubSim::syncInitEP() {
    pthread_barrier_destroy(&_barrier);
    pthread_barrier_init(&_barrier, nullptr, 2);
    _evSyncMaster = getSimulator()->GetScheduler()->AddSimulationTimeEvent(_epSync, 0, _syncRate);
    _evSyncSub = _subSim->GetScheduler()->AddSimulationTimeEvent(_epSync, 0, _syncRate);
    _subSim->Run();
}
// ..........................................................
void SyncSubSim::syncReleaseEP() {
    _subSim->Hold(true);
    getSimulator()->GetScheduler()->RemoveEvent(_evSyncMaster);
    _subSim->GetScheduler()->RemoveEvent(_evSyncSub);
    pthread_barrier_destroy(&_barrier);
    pthread_barrier_init(&_barrier, nullptr, 0);
}

}  // namespace mt
}  // namespace simph

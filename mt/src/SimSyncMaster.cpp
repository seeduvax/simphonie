/*
 * @file SimSyncMaster.cpp
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <sstream>
#include "simphonie/mt/SimSyncMaster.hpp"
#include "Smp/IPublication.h"
#include "Smp/Services/IScheduler.h"
#include "Smp/Services/IEventManager.h"

#include <thread>

#define EP_SYNC "sync"
#define EP_INIT "init"
#define EP_STORE "store"
#define EP_RESTORE "restore"
#define EP_RUN "run"
#define EP_LEAVERUN "leaverun"
#define EP_EXIT "exit"
#define EP_ABORT "abort"

namespace simphonie {
namespace mt {

SimSyncMaster::SimSyncMaster(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : simdeck::Service(name, descr, parent)
{
    addEP(EP_SYNC, "Simulators' synchronization function. Blocks until every slaveulators are waiting to then share data among them.", this, &SimSyncMaster::sync);
    addEP(EP_INIT, "Internal use only.", this, &SimSyncMaster::init);
    addEP(EP_STORE, "Internal use only.", this, &SimSyncMaster::store);
    addEP(EP_RESTORE, "Internal use only.", this, &SimSyncMaster::restore);
    addEP(EP_RUN, "Internal use only.", this, &SimSyncMaster::run);
    addEP(EP_LEAVERUN, "Internal use only.", this, &SimSyncMaster::leaverun);
    addEP(EP_EXIT, "Internal use only.", this, &SimSyncMaster::exit);
    addEP(EP_ABORT, "Internal use only.", this, &SimSyncMaster::abort);
}

void SimSyncMaster::publish(Smp::IPublication* receiver) {
    receiver->PublishArray("slaves", "Sub-simulators", 1, _slavesAddr, Smp::PrimitiveTypeKind::PTK_Int64,
        Smp::ViewKind::VK_All, false, true, false); /* assuming 64bits long pointers */
}

void SimSyncMaster::configure() {
    for (size_t i = 0; i < 1; ++i) {
        const auto slave = dynamic_cast<SimSyncSlave*>(_slavesAddr[i]);
        if (slave) {
            _slaves.push_back(slave);
            slave->setBarrier(&_barrier);
           slave->getSim()->Configure();
        }
    }
    _barrier.setThreshold(_slaves.size() + 1);
}

void SimSyncMaster::connect() {
    for (const auto slave : _slaves) {
        slave->getSim()->Connect();
    }
    getSimulator()->AddInitEntryPoint(GetEntryPoint(EP_INIT));
    getSimulator()->GetEventManager()->Subscribe(Smp::Services::IEventManager::SMP_EnterStoringId, GetEntryPoint(EP_STORE));
    getSimulator()->GetEventManager()->Subscribe(Smp::Services::IEventManager::SMP_EnterRestoringId, GetEntryPoint(EP_RESTORE));
    getSimulator()->GetEventManager()->Subscribe(Smp::Services::IEventManager::SMP_EnterExecutingId, GetEntryPoint(EP_RUN));
    getSimulator()->GetEventManager()->Subscribe(Smp::Services::IEventManager::SMP_LeaveExecutingId, GetEntryPoint(EP_LEAVERUN));
    getSimulator()->GetEventManager()->Subscribe(Smp::Services::IEventManager::SMP_EnterExitingId, GetEntryPoint(EP_EXIT));
    getSimulator()->GetEventManager()->Subscribe(Smp::Services::IEventManager::SMP_EnterAbortingId, GetEntryPoint(EP_ABORT));
}

void SimSyncMaster::init() {
    for (const auto slave : _slaves) {
        slave->getSim()->Initialise();
    }
}

void SimSyncMaster::store() {
    for (const auto slave : _slaves) {
        std::ostringstream oss;
        oss << slave->getSim()->GetName() << ".cp";
        slave->getSim()->Store(oss.str().c_str()); /* TODO filename, maybe same file as the master one */
    }
}

void SimSyncMaster::restore() {
    for (const auto slave : _slaves) {
        std::ostringstream oss;
        oss << slave->getSim()->GetName() << ".cp";
        slave->getSim()->Restore(oss.str().c_str()); /* TODO filename, maybe same file as the master one */
    }
}

void SimSyncMaster::sync() {
    if (_barrier.wait()) {
        /* Share data here */
    }
}

void SimSyncMaster::run() {
    _barrier.reset();
    for (const auto slave : _slaves) {
        slave->getSim()->Run();
    }
}

void SimSyncMaster::leaverun() {
    for (const auto slave : _slaves) {
        slave->setExitFlag(SimSyncSlave::exitFlags::HOLD);
    }
    _barrier.cancel();
    for (const auto slave : _slaves) {
        while (slave->getSim()->GetState() != Smp::SimulatorStateKind::SSK_Standby) {}
    }
}

void SimSyncMaster::exit() {
    for (const auto slave : _slaves) {
        slave->setExitFlag(SimSyncSlave::exitFlags::EXIT);
    }
    _barrier.cancel();
    for (const auto slave : _slaves) {
        while (slave->getSim()->GetState() != Smp::SimulatorStateKind::SSK_Standby) {}
    }
}

void SimSyncMaster::abort() {
    for (const auto slave : _slaves) {
        slave->setExitFlag(SimSyncSlave::exitFlags::ABORT);
    }
    _barrier.cancel();
    for (const auto slave : _slaves) {
        while (slave->getSim()->GetState() != Smp::SimulatorStateKind::SSK_Standby) {}
    }
}

}  /* namespace mt */
}  /* namespace simphonie */

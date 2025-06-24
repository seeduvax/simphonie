/*
 * @file SimSyncMaster.cpp
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/mt/SimSyncMaster.hpp"
#include "Smp/IPublication.h"
#include "Smp/Services/IEventManager.h"
#include "Smp/Services/IScheduler.h"
#include "simdeck/ExInvalidFieldName.hpp"

#define EP_SYNC "sync"
#define EP_INIT "init"
#define EP_STORE "store"
#define EP_RESTORE "restore"
#define EP_RUN "run"
#define EP_LEAVERUN "leaverun"
#define EP_EXIT "exit"
#define EP_ABORT "abort"
#define EP_SLAVESTBY "slavestdby"

namespace simphonie {
namespace mt {

SimSyncMaster::SimSyncMaster(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : simdeck::Service(name, descr, parent), _barrier(2) {
    addEP(EP_SYNC,
          "Simulators' synchronization function. Blocks until every simulators are waiting to then share data among "
          "them.",
          this, &SimSyncMaster::sync);
    addEP(EP_INIT, "Internal use only.", this, &SimSyncMaster::init);
    addEP(EP_STORE, "Internal use only.", this, &SimSyncMaster::store);
    addEP(EP_RESTORE, "Internal use only.", this, &SimSyncMaster::restore);
    addEP(EP_RUN, "Internal use only.", this, &SimSyncMaster::run);
    addEP(EP_LEAVERUN, "Internal use only.", this, &SimSyncMaster::leaverun);
    addEP(EP_EXIT, "Internal use only.", this, &SimSyncMaster::exit);
    addEP(EP_ABORT, "Internal use only.", this, &SimSyncMaster::abort);
    addEP(EP_SLAVESTBY, "Internal use only.", this, &SimSyncMaster::slavestby);
}

SimSyncDataShare::DataType SimSyncMaster::sendData() {
    std::lock_guard<std::mutex> lock(_dataMtx);
    return _data;
}

void SimSyncMaster::publish(Smp::IPublication* receiver) {
    receiver->PublishField("slave", "Sub-simulator", &_slaveAddr, Smp::ViewKind::VK_All, false, true, false);
    _dataShare.publish(this, receiver);
}

void SimSyncMaster::configure() {
    _slave = reinterpret_cast<SimSyncSlave*>(static_cast<uintptr_t>(_slaveAddr));
    _slave->setBarrier(&_barrier);
    _slave->setSendDataFunc(std::bind(&SimSyncMaster::sendData, this));
    _slave->getSim()->Configure();
}

void SimSyncMaster::connect() {
    _slave->getSim()->Connect();
    getSimulator()->AddInitEntryPoint(GetEntryPoint(EP_INIT));
    getSimulator()->GetEventManager()->Subscribe(Smp::Services::IEventManager::SMP_EnterStoringId, GetEntryPoint(EP_STORE));
    getSimulator()->GetEventManager()->Subscribe(Smp::Services::IEventManager::SMP_EnterRestoringId, GetEntryPoint(EP_RESTORE));
    getSimulator()->GetEventManager()->Subscribe(Smp::Services::IEventManager::SMP_EnterExecutingId, GetEntryPoint(EP_RUN));
    getSimulator()->GetEventManager()->Subscribe(Smp::Services::IEventManager::SMP_LeaveExecutingId, GetEntryPoint(EP_LEAVERUN));
    getSimulator()->GetEventManager()->Subscribe(Smp::Services::IEventManager::SMP_EnterExitingId, GetEntryPoint(EP_EXIT));
    getSimulator()->GetEventManager()->Subscribe(Smp::Services::IEventManager::SMP_EnterAbortingId, GetEntryPoint(EP_ABORT));
    _resolver = getSimulator()->GetResolver();
}

void SimSyncMaster::init() {
    _slave->getSim()->Initialise();
}

void SimSyncMaster::store() {
    std::ostringstream oss;
    oss << _slave->getSim()->GetName() << ".cp";
    _slave->getSim()->Store(oss.str().c_str()); /* TODO filename, maybe same file as the master one */
}

void SimSyncMaster::restore() {
    std::ostringstream oss;
    oss << _slave->getSim()->GetName() << ".cp";
    _slave->getSim()->Restore(oss.str().c_str()); /* TODO filename, maybe same file as the master one */
}

void SimSyncMaster::sync() {
    {
        const auto data = _dataShare.retrieveData();
        std::lock_guard<std::mutex> lock(_dataMtx);
        _data = data;
    }
    if (_barrier.wait()) {
        const auto data = _slave->sendData();
        _dataShare.loadData(data);
    }
}

void SimSyncMaster::run() {
    _barrier.reset();
    _slave->getSim()->Run();
}

void SimSyncMaster::waitSlaveStandby() {
    _slave->getSim()->GetEventManager()->Subscribe(Smp::Services::IEventManager::SMP_EnterStandbyId,
                                                   GetEntryPoint(EP_SLAVESTBY));
    std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);
    _condvar.wait(lock, [&] { return _slave->getSim()->GetState() == Smp::SimulatorStateKind::SSK_Standby; });
    _slave->getSim()->GetEventManager()->Unsubscribe(Smp::Services::IEventManager::SMP_EnterStandbyId,
                                                     GetEntryPoint(EP_SLAVESTBY));
}

void SimSyncMaster::slavestby() {
    _condvar.notify_one();
}

void SimSyncMaster::leaverun() {
    _slave->setExitFlag(SimSyncSlave::exitFlags::HOLD);
    _barrier.cancel();
    waitSlaveStandby();
}

void SimSyncMaster::exit() {
    _slave->setExitFlag(SimSyncSlave::exitFlags::EXIT);
    _barrier.cancel();
    waitSlaveStandby();
}

void SimSyncMaster::abort() {
    _slave->setExitFlag(SimSyncSlave::exitFlags::ABORT);
    _barrier.cancel();
    waitSlaveStandby();
}

} /* namespace mt */
} /* namespace simphonie */

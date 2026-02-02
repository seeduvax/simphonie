/*
 * @file SimFront.cpp
 *
 * Copyright 2026 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simdeck/front/SimFront.hpp"
#include "simdeck/EntryPoint.hpp"
#include "Smp/Services/IEventManager.h"
#include <condition_variable>
#include <mutex>

namespace simdeck {
namespace front {

class EndSimTime: public simdeck::EntryPoint {
public:
    EndSimTime(SimFront* front):
                simdeck::EntryPoint("sim_front_end_sim_time__","",nullptr),
                _front(front) {
    }
    virtual ~EndSimTime() {
    }

    void Execute() const override {
        auto s=_front->GetSimulator();
        auto endSimTime=_front->GetEndSimulationTime();
        if (endSimTime>0 && s->GetTimeKeeper()->GetSimulationTime() >= endSimTime) {
            s->Hold(true);
        }
    }
private:
    SimFront* _front;
};

class StateWaiter: public simdeck::EntryPoint {
public:
    StateWaiter(Smp::ISimulator* sim, Smp::SimulatorStateKind state):
                simdeck::EntryPoint("sim_front_state_waiter__","",nullptr),
                _sim(sim),
                _expState(state) {
        auto em=_sim->GetEventManager();
        em->Subscribe(Smp::Services::IEventManager::SMP_EnterInitialisingId, this);
        em->Subscribe(Smp::Services::IEventManager::SMP_EnterStandbyId, this);
        em->Subscribe(Smp::Services::IEventManager::SMP_EnterExecutingId, this);
        em->Subscribe(Smp::Services::IEventManager::SMP_EnterStoringId, this);
        em->Subscribe(Smp::Services::IEventManager::SMP_EnterRestoringId, this);
        em->Subscribe(Smp::Services::IEventManager::SMP_EnterReconnectingId, this);
        em->Subscribe(Smp::Services::IEventManager::SMP_EnterExitingId, this);
        em->Subscribe(Smp::Services::IEventManager::SMP_EnterAbortingId, this);
    }
    virtual ~StateWaiter() {
        auto em=_sim->GetEventManager();
        em->Unsubscribe(Smp::Services::IEventManager::SMP_EnterInitialisingId, this);
        em->Unsubscribe(Smp::Services::IEventManager::SMP_EnterStandbyId, this);
        em->Unsubscribe(Smp::Services::IEventManager::SMP_EnterExecutingId, this);
        em->Unsubscribe(Smp::Services::IEventManager::SMP_EnterStoringId, this);
        em->Unsubscribe(Smp::Services::IEventManager::SMP_EnterRestoringId, this);
        em->Unsubscribe(Smp::Services::IEventManager::SMP_EnterReconnectingId, this);
        em->Unsubscribe(Smp::Services::IEventManager::SMP_EnterExitingId, this);
        em->Unsubscribe(Smp::Services::IEventManager::SMP_EnterAbortingId, this);
    }

    void Execute() const override {
        std::unique_lock<std::mutex> sync(_mutex);
        if (_sim->GetState()==_expState) {
            _monitor.notify_all();
        }
    }

    Smp::Bool wait(Smp::Duration timeout=0) {
        std::unique_lock<std::mutex> sync(_mutex);
        auto cvStatus=std::cv_status::no_timeout;
        auto maxDate=std::chrono::high_resolution_clock::now() + std::chrono::nanoseconds(timeout);
        while  (_sim->GetState()!=_expState && cvStatus==std::cv_status::no_timeout) {
            if (timeout==0) {
                _monitor.wait(sync);
            }
            else {
                cvStatus=_monitor.wait_until(sync, maxDate);
            }
        }
        return cvStatus==std::cv_status::no_timeout;
    }

    
private:
    Smp::ISimulator* _sim;
    Smp::SimulatorStateKind _expState;
    mutable std::mutex _mutex;
    mutable std::condition_variable _monitor;
};

// --------------------------------------------------------------------
// ..........................................................
SimFront::SimFront(Smp::ISimulator* sim): _sim(sim) {
    _endSimTime=new EndSimTime(this);
    _sim->GetEventManager()->Subscribe(
            Smp::Services::IEventManager::SMP_PostSimTimeChangeId,
            _endSimTime);
}
// ..........................................................
SimFront::~SimFront() {
    _sim->GetEventManager()->Unsubscribe(
            Smp::Services::IEventManager::SMP_PostSimTimeChangeId,
            _endSimTime);
    delete _endSimTime;
}
// --------------------------------------------------------------------
// ..........................................................
Smp::Bool SimFront::Wait(Smp::SimulatorStateKind state, Smp::Duration zuluTimeout) {
    StateWaiter w(_sim,state);
    return w.wait(zuluTimeout);
}

}} // namespace simdeck::front

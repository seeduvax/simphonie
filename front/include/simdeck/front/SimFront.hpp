/*
 * @file SimFront.hpp
 *
 * Copyright 2026 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_front_SimFront_HPP__
#define __simdeck_front_SimFront_HPP__

#include "Smp/ISimulator.h"
#include "Smp/Services/ITimeKeeper.h"
#include "Smp/Services/IResolver.h"
#include "Smp/IModel.h"
#include "simdeck/Utils.hpp"

namespace simdeck {
namespace front {

/**
 * Simulator front interface and control helper.
 * Wrapping interface to add utility methods for easier control of the
 * simulation, for instance wait for simulation end just after it was started.
 */
class SimFront {
public:
    /**
     * Create new front interface wrapping a simulator
     * @param sim SMP simulator to wrap.
     */
    SimFront(Smp::ISimulator* sim);
    /**
     * Destructor.
     */
    virtual ~SimFront();

    /**
     * @return the simulator linked to this front interface.
     */
    inline Smp::ISimulator* GetSimulator() const {
        return _sim;
    }
    /**
     * Initialize the simulation.
     * Perform all the steps required from simulator creation to its stand by
     * state ready for starting the simulation.
     */
    inline void Init() {
        _sim->Publish();
        _sim->Configure();
        _sim->Connect();
    }

    inline void Run() {
        _sim->Run();
    }
    inline void Hold(Smp::Bool immediate=false) {
        _sim->Hold(immediate);
    }
    /**
     * Print on console the simulator structure.
     * Print the tree of components found in the simulator.
     * @param o tree root: objet from which dumm the tree, default is nullptr
     *   meaning dump all from the wrapped simulator itself.
     */
    inline void Dump(Smp::IObject* o=nullptr) {
        simdeck::Utils::Dump(o==nullptr?_sim:o, 0, _sim);
    }
    /**
     * Wait for the simulator reaches a specific state.
     * For instance to wait for simulation end, wait for the SSK_StandBy state.
     * @param state state to wait for
     * @param zuluTime maximum relative time from now (with zulu clock) to give
     *  up for the waiting. When 0, no timeout and the call may block 
     *  indefinitely.
     * @return
     *  - true: when state is reached.
     *  - false: wait exit on timeout.
     */
    Smp::Bool Wait(Smp::SimulatorStateKind state, Smp::Duration zuluTimeout=0);
    /**
     * Set the simulation end simulation time.
     */
    inline void SetEndSimulationTime(Smp::Duration endTime) {
        _endSimulationTime = endTime;
    }
    /**
     * Set the simulation duration.
     * Configure the simulation to end automatically when the simulation time 
     * reaches the current time keeper value + the duration given in argument.
     */
    inline void SetSimulationDuration(Smp::Duration duration) {
        SetEndSimulationTime(_sim->GetTimeKeeper()->GetSimulationTime()+duration);
    }

    /**
     * Get the currently applied end simulation time.
     * @return end simulation time value.
     */
    inline Smp::Duration GetEndSimulationTime() const {
        return _endSimulationTime;
    }

    /**
     * Get a model by name.
     * @param fullName searched model's full name.
     * @return found model, nullptr if none. 
     */
    inline Smp::IModel* GetModel(Smp::String8 fullName) {
        return dynamic_cast<Smp::IModel*>(
            _sim->GetResolver()->ResolveAbsolute(fullName));
    }

    /**
     * Get the simulator's top level services component collection.
     * @return list of services as component collection
     */
    inline const Smp::ComponentCollection* GetServices() {
        return _sim->GetContainer(Smp::ISimulator::SMP_SimulatorServices)->GetComponents();
    } 

    /**
     * Get the simulator's top level models component collection.
     * @return list of models as component collection
     */
    inline const Smp::ComponentCollection* GetModels() {
        return _sim->GetContainer(Smp::ISimulator::SMP_SimulatorModels)->GetComponents();
    } 

    /**
     * Find a service by type.
     * @return 1st service matching the type, nullptr if none.
     */
    template <typename T> 
    T* GetService() {
        for (auto svc: *(_sim->GetContainer(Smp::ISimulator::SMP_SimulatorServices)->GetComponents())) {
            auto found=dynamic_cast<T*>(svc);
            if (found!=nullptr) {
                return found;
            }
        }
        return nullptr;
    }

private:
    Smp::ISimulator* _sim=nullptr;
    Smp::Duration _endSimulationTime=0;
    Smp::IEntryPoint* _endSimTime;
};

}} // namespace simdeck::front
#endif // __simdeck_front_SimFront_HPP__

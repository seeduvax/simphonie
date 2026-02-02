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

    inline Smp::Duration GetEndSimulationTime() const {
        return _endSimulationTime;
    }

private:
    Smp::ISimulator* _sim=nullptr;
    Smp::Duration _endSimulationTime=0;
    Smp::IEntryPoint* _endSimTime;
};

}} // namespace simdeck::front
#endif // __simdeck_front_SimFront_HPP__

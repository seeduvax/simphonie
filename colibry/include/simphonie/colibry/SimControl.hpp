/*
 * @file SimControl.hpp
 *
 * Copyright 2025 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_colibry_SimControl_HPP__
#define __simphonie_colibry_SimControl_HPP__

#include "simphonie/colibry/Tracker.hpp"

namespace simphonie {
namespace colibry {


/**
 * Control simulation end on field expression.
 * This service use the Tracker service capabilities to track fields and detect
 * specific simulation configuration based on expression (defined with
 * S-Expression feature inherited from Tracker).
 *
 * The service register itself to the SMP_PostSimTimeChange event, then
 * the expression and decision to stop the simulation is evaluated once for 
 * every simulation time value. When the S-Expression result (supposed to be a
 * boolean expression even the expression engine primary type is double) is not
 * zero, the service request the host simulator to hold, then by the way
 * stopping the simulation.
 *
 * Many instances of the SimControl can be added to the simulator when any of
 * many criteria (with one SimControl instance for each criteria) shall be
 * verified to stop the simulation.
 *
 * See also the `Tracker` class documentation for setting and initializing the
 * service since the behavior of the `SimControl` is inherited from `Tracker`
 * regarding the expression handling.
 */ 
class SimControl : public Tracker {
    typedef Tracker Parent;
public:
    /**
     * Sim Control main constructor.
     * @param name instance name.
     * @param descr instance description
     * @param parent Parent SMP Object. As the SimControl is a Service, in most
     * case the parent object is the host simulator itself.
     */ 
    SimControl(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent);
    ~SimControl();

protected:
    /**
     * SMP component Connect specialization.
     * On connct, the SimControl service registers itself its expression
     * evaluation update entry point to the event manager's 
     * SMP_PostSimTimeChange event.
     */ 
    void connect() override;

    /**
     * SMP component Disonnect specialization.
     * Unsubscribe from what was subscribed on connect.
     */
    void disconnect() override;
    /** 
     * Tracker onEvaluate callback specialisation.
     * When expression result is updated, the host simulator Hold is requested
     * if expression result is not zero.
     */ 
    void onEvaluate() override;
};

} /* namespace colibry */
} /* namespace simphonie */
#endif /* __simphonie_colibry_SimControl_HPP__ */

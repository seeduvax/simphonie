/*
 * @file IMetaScheduler.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_smpext_IMetaScheduler_HPP__
#define __simdeck_smpext_IMetaScheduler_HPP__

#include "Smp/IService.h"
#include "Smp/Services/EventId.h"
#include "Smp/IEntryPoint.h"

namespace simdeck {
namespace smpext {

/**
 * The meta scheduler service add on event scheduling features.
 * The meta scheduler service use the SMP scheduler as a backend to delegate
 * the concrete schedulong of entry points. It provides a new entry point
 * schedule interface to define the scheduling rules with additional attributes
 * realted to events enabling definition of the following schedule:
 * - cyclic schedule that shall be active from one event until another event 
 *   (event manager's events to trig activation and deactivation).
 * - acyclic schedule triggered by entry point. The difference with direct
 *   registration of the entry point to the event manager is to have an
 *   execution order relative to the other entry point scheduled at the time
 *   the event is emitted, defined by the addition (submit) order.
 * - any combination of the above with the usual SMP timed and cyclic scheduling
 * - immediate events with event based active control. when not active, the
 *   addition of the immediate event is ignored.    
 */
class IMetaScheduler: public virtual Smp::IService  {
public:
    /**
     * Destructor.
     */
    virtual ~IMetaScheduler() noexcept = default;

    /**
     * Entry point extended schedule.
     * Once created the the IMetaScheduler::NewSchedule factory method, the
     * schedule is configured using the many Set method that can be chained
     * (see examples from IMetaScheduler interface documentation. When submit
     * is called, the schedule (and its entry point) is finally added to the
     * scheduler (when not added yet) or the scheduler event is reconfigured
     * with the new attributes values (given with the Set methods) received
     * since the previous submit count.
     */
    class ISchedule {
    public:
        virtual ~ISchedule() noexcept = default;
        /**
         * Set the simulation time at which the entry point shall run.
         * @param simTime simulation time, relative to the current time keeper
         * time
         * @return this schedule for further configuration.
         */ 
        virtual ISchedule& SetSimulationTime(Smp::Duration simTime) = 0;
        /**
         * Set the cycle time for cyclic events.
         * @param cycleTime cycle time.
         * @return this schedule for further configuration.
         */
        virtual ISchedule& SetCycleTime(Smp::Duration cycleTime) = 0;
        /**
         * Set repeat count.
         * @param repeat when >0 the total entry point execution count is 
         *    repeat + 1. When =0 the entry point is executed once. When =-1
         *    the entry point is activated periodically until the simulation end
         *    or any registered event manager stop event.
         * @return this schedule for further configuration.
         */
        virtual ISchedule& SetRepeat(Smp::Int64 repeat) = 0;
        /**
         * Set active flag
         * @param active 
         *   - true the entry point is active as soon it is submitted.
         *   - false the entry point is in active and will not be executed until
         *     a registered event manager activate event is emitted or SetActive
         *     called again with true then submitted again.
         * @return this schedule for further configuration.
         */
        virtual ISchedule& SetActive(Smp::Bool active) = 0;
        /**
         * Register to an activate event.
         * @param event Event manager's event id to expect as a activate event.
         * @return this schedule for further configuration.
         */
        virtual ISchedule& SubscribeActivateEvent(Smp::Services::EventId event) = 0;
        /**
         * Cancel resgistration to an activate event.
         * @param event Event manager's event id to unsubscribe from for
         * activation.
         * @return this schedule for further configuration.
         */
        virtual ISchedule& UnsubscribeActivateEvent(Smp::Services::EventId event) = 0;
        /**
         * Register to a deactivate event.
         * @param event Event manager's event id to expect as a deactivate
         * event.
         * @return this schedule for further configuration.
         */
        virtual ISchedule& SubscribeDeactivateEvent(Smp::Services::EventId event) = 0;
        /**
         * Cancel resgistration to a deactivate event.
         * @param event Event manager's event id to unsubscribe from for
         * deactivation.
         * @return this schedule for further configuration.
         */
        virtual ISchedule& UnsubscribeDeactivateEvent(Smp::Services::EventId event) = 0;
        /**
         * Register to an event to trigger a cycle time change.
         * @param event Event manager's event id to subsbscribe to.
         * @param cycleTime new cycle time to set after event is emmitted.
         * @param delay simulation time delay before cycle time change 
         *        application from the time the event is emitted.
         * @return this schedule for further configuration.
         */
        virtual ISchedule& SubscribeCycleTimeChangeEvent(
                            Smp::Services::EventId event,
                            Smp::Duration cycleTime,
                            Smp::Duration delay = 0) = 0;
        /**
         * Cancel event subscription for a cycle time change.
         * @return this schedule for further configuration.
         */
        virtual ISchedule& UnsubscribeCycleTimeChangeEvent(Smp::Services::EventId event) = 0;
        /**
         * Submit the schedule to the Smp scheduler.
         * Translate the schedule configuration to a SMP schedule add event call
         * or already scheduled event attributes update sequence.
         */ 
        virtual void Submit() = 0;
        /**
         * @return the scheduler event id:
         *   - -1 until the schedule is submitted.
         *   - event id assigned by the scheduler on addition:
         *     - last addition from last submit for immediate schedule.
         *     - value obtained of on addition from the first submit.
         */   
        virtual Smp::Services::EventId GetEventId() const  = 0;
        /**
         * @return event type immediate (true) or not (false).
         */
        virtual Smp::Bool IsImmediate()=0;
        /**
         * @return simulation time. Should be 0 for immediate events.
         */
        virtual Smp::Duration GetSimulationTime()=0;
        /**
         * @return cycle time.
         */
        virtual Smp::Duration GetCycleTime()=0;
        /**
         * @return repeat count.
         */
        virtual Smp::Int64 GetRepeat()=0;
        /**
         * @return current active status.
         */
        virtual Smp::Bool IsActive()=0;
    };

    /**
     * Meta Scheduler schedule activation listener.
     * Enables to track execution of entry points scheduled with the
     * meta scheduler.
     */
    class IScheduleListener {
    public:
        virtual ~IScheduleListener() noexcept = default;
        /**
         * scheduled entry point execution start notification.
         * Called just before the entry point execution starts.
         */
        virtual void NotifyExecBegin(const ISchedule* s) = 0;
        /**
         * scheduled entry point execution end notification.
         * Called just after the entry point execution is completed.
         */
        virtual void NotifyExecEnd(const ISchedule* s) = 0;
    };

    /**
     * Retrieve a meta scheduler schedule from a SMP scheduler event Id.
     * @param id schedule event id to search for.
     * @return pointer to found schedule, or nullptr when not found.
     */
    virtual ISchedule* GetSchedule(Smp::Services::EventId id) const = 0;
    /**
     * Create a new meta scheduler schedule.
     * @param ep entry point to be scheduled.
     * @param immediate
     *   - true: request immediate schedule.
     *   - false: request simulation time schedule.
     */
    virtual ISchedule* NewSchedule(Smp::IEntryPoint* ep, Smp::Bool immediate=false) = 0;
};

}} // namespace simdeck::smpext
#endif // __simdeck_smpext_IMetaScheduler_HPP__

/*
 * @file IObservableScheduler.hpp
 *
 * Copyright 2025 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_smpext_IObservableScheduler_HPP__
#define __simdeck_smpext_IObservableScheduler_HPP__

#include "Smp/IComponent.h"
#include "simdeck/smpext/ISchedule.hpp"
#include "simdeck/smpext/ISchedulerObserver.hpp"

namespace simdeck {
namespace smpext {

/**
 * Smp::IScheduler extension to enable listening for the scheduler activity, and
 * more specifically the changes in the schedule queue.
 * All the registered observers are notified according the ISchedulerObserver
 * interface when:
 *  - a new event is scheduled in the scheduler.
 *  - a schedule event is completed (its repetition reached 0).
 *  - a schedule event is canceled.
 * Additionaly to complete observability, the current schedule event descriptor
 * can be fetched, in order to, coupled with proper event subscription trigger 
 * some action pre and post entry point execution by the scheduler knowing what
 * is the to-be-executed / executed entry point.
 * By doing so if some simulation supervisor wish to know what is the current 
 * status of the scheduler, it may be achieved observing the scheduler and 
 * collecting the information, instead of requesting the content of the schedule
 * queue concurrently with the scheduler thread updating it.
 * The other common use is to have some entry point execution statistic service. 
 * TODO: specify when and what thread shall notify 
 *   - immadiatly by the thread requesting the addition of event to the 
 *     scheduler
 *   - slightly delayed by the scheduler thread, when the schedule queue is 
 *     updated and ready to process the added events.
 * TODO: decide if this interface shall extend:
 *   - Smp::IScheduler
 *   - Smp::Icomponent
 *   - Smp::IObject
 *   - nothing (may be an issue to let users check if the scheduler instance of
 *     a simulator implement this interface or not).
 */       
class IObservableScheduler : virtual public Smp::IComponent {
public:
    virtual ~IObservableScheduler() noexcept = default;

    /**
     * Register a scheduler observer.
     * @param observer observer to be registered and that may be notified once
     * registered on specific events (see class intro).
     */  
    virtual void RegisterObserver(ISchedulerObserver* observer) = 0;
    /**
     * Unregister a scheduler observer.
     * @param observer the observer that shall no longer be notified of the
     * schedule queue changes.
     */ 
    virtual void RemoveObserver(ISchedulerObserver* observer) = 0;
    /**
     * Get the current schedule event descriptor.
     * Caution: shall be called only from the scheduler thread:
     *   - letting an entry point know for instance its period, or remaining
     *     activation count.
     *   - let a component subscribed to pre/post entry point execuction events
     *     (if any fired by the scheduler implementation) do some actions pre/
     *     post entry point exec knowing its schedule parameter (for instance to
     *     collect statistics on entry points execution, or to dynamically 
     *     extend actions related to an entry point execution).
     *  @return the current scheduled event's descriptor. Its validity at the
     *  time the methods is called and returned is scheduler implementation
     *  specific and the behaviour can't be rigorously defined for a call
     *  outside the scheduler thread.
     */   
    virtual const ISchedule* GetSchedule() const = 0;
};

} /* namespace smpext */
} /* namespace simdeck */

#endif /* __simdeck_smpext_IObservableScheduler_HPP__ */

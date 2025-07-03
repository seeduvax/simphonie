/*
 * @file ISchedulerObserver.hpp
 *
 * Copyright 2025 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_smpext_ISchedulerObserver_HPP__
#define __simdeck_smpext_ISchedulerObserver_HPP__

#include "Smp/Services/EventId.h"
#include "simdeck/smpext/ISchedule.hpp"

namespace simdeck {
namespace smpext {

/**
 * Schedule queue change listening interface.
 */ 
class ISchedulerObserver {
public:
    virtual ~ISchedulerObserver() noexcept = default;


    /**
     * Notify a new event addition in the schedule queue.
     * The observable scheduler shall call back this notification method each
     * time a new event is added to the scheduler. (TODO define if done
     * immediately or by the scheduler thread then slightly delayed).
     * @param event schedule event descriptor.
     */   
    virtual void notifyScheduled(const ISchedule *event) = 0;
    /**
     * Notify a schedule event is completed.
     * The observable scheduler shall call back thos ntotification method each
     * time a scheduled event is fully achieved, meaning the repeat counter
     * as reached zero after the entry point execution and will no longer
     * activate the entry point.
     * @param eventId id of the scheduled event that is completed.
     */ 
    virtual void notifyCompleted(Smp::Services::EventId eventId) = 0;
    /**
     * Notify a scheduled event is canceled.
     * The observable scheduler shall call back this notification method each
     * time the cancelation of the schedule event is requested (TBD define if 
     * done immediately or by the scheduler thread then slightly delayed).
     * @param eventId id of the cancelled event
     */ 
    virtual void notifyCanceled(Smp::Services::EventId eventId) = 0;

};

} /* namespace smpext */
} /* namespace simdeck */

#endif /* __simdeck_smpext_ISchedulerObserver_HPP__ */

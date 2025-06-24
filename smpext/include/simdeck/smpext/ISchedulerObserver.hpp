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

class ISchedulerObserver {
public:
    virtual ~ISchedulerObserver() noexcept = default;

    virtual void notifyScheduled(const ISchedule *event) = 0;
    virtual void notifyUpdated(Smp::Services::EventId eventId) = 0;
    virtual void notifyCompleted(Smp::Services::EventId eventId) = 0;
    virtual void notifyCanceled(Smp::Services::EventId eventId) = 0;

};

} /* namespace smpext */
} /* namespace simdeck */

#endif /* __simdeck_smpext_ISchedulerObserver_HPP__ */

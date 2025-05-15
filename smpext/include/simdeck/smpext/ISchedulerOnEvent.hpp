/*
 * @file ISchedulerOnEvent.hpp
 *
 * Copyright 2025 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_smpext_ISchedulerOnEvent_HPP__
#define __simdeck_smpext_ISchedulerOnEvent_HPP__

#include "Smp/Services/EventId.h"

namespace simdeck {
namespace smpext {

class ISchedulerOnEvent {
public:
    virtual ~ISchedulerOnEvent() noexcept = default;

    virtual void SetEventStartOnEvent(Smp::Services::EventId eventId,
        Smp::Services::EventId triggerEventId) = 0;

    virtual void SetEventStopOnEvent(Smp::Services::EventId eventId,
        Smp::Services::EventId triggerEventId) = 0;

};

} /* namespace smpext */
} /* namespace simdeck */

#endif /* __simdeck_smpext_ISchedulerOnEvent_HPP__ */

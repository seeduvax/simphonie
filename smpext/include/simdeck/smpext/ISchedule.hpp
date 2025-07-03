/*
 * @file ISchedule.hpp
 *
 * Copyright 2025 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_smpext_ISchedule_HPP__
#define __simdeck_smpext_ISchedule_HPP__

#include "Smp/IObject.h"
#include "Smp/Services/EventId.h"

namespace simdeck {
namespace smpext {

/**
 * Schedule event descriptor.
 */ 
class ISchedule {
public:
    virtual ~ISchedule() noexcept = default;

    /**
     * @return schedule event id.
     */ 
    virtual Smp::Services::EventId GetId() const = 0;
    /**
     * @return entry point.
     */ 
    virtual const Smp::IEntryPoint* GetEP() const = 0;
    /**
     * @return next activation time (in simulation time reference)
     */   
    virtual Smp::Duration GetTime() const = 0;
    /**
     * @return id of event switch this schedule from off to on.
     */ 
    virtual Smp::Services::EventId GetStartEventId() const = 0;
    /**
     * @return id of event switch this schedule from on to off.
     */ 
    virtual Smp::Services::EventId GetStopEventId() const = 0;
    /**
     * @return entry point activation period.
     */  
    virtual Smp::Duration GetPeriod() const = 0;
    /**
     * @return remaining activation count.(TODO check this is really that or initial repeatition count)
     */ 
    virtual Smp::Int64 GetRepeat() const = 0;
    /**
     * @return done activation count
     */ 
    virtual Smp::Int64 GetActivationCounter() const = 0;
    /**
     * @return completion status. true when 
     *   - all repetition done, 
     *   - (TODO confirms this, unsure because start event may be emitted again...)
     *      when stop event has been triggered 
     */ 
    virtual Smp::Bool IsCompleted() const = 0;

};

} /* namespace smpext */
} /* namespace simdeck */

#endif /* __simdeck_smpext_ISchedule_HPP__ */

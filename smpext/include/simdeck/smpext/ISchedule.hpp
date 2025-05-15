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

class ISchedule : virtual public Smp::IObject {
public:
    virtual ~ISchedule() noexcept = default;

    virtual Smp::Services::EventId GetId() const = 0;
    virtual const Smp::IEntryPoint* GetEP() const = 0;
    virtual Smp::Duration GetTime() const = 0;
    virtual Smp::Services::EventId GetStartEventId() const = 0;
    virtual Smp::Services::EventId GetStopEventId() const = 0;
    virtual Smp::Duration GetPeriod() const = 0;
    virtual Smp::Int64 GetRepeat() const = 0;
    virtual Smp::Int64 GetActivationCounter() const = 0;
    virtual Smp::Bool IsCompleted() const = 0;

};

} /* namespace smpext */
} /* namespace simdeck */

#endif /* __simdeck_smpext_ISchedule_HPP__ */

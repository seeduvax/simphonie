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

class IObservableScheduler : virtual public Smp::IComponent {
public:
    virtual ~IObservableScheduler() noexcept = default;

    virtual void RegisterObserver(ISchedulerObserver* observer) = 0;
    virtual void RemoveObserver(ISchedulerObserver* observer) = 0;

    virtual const ISchedule* GetSchedule() const = 0;

};

} /* namespace smpext */
} /* namespace simdeck */

#endif /* __simdeck_smpext_IObservableScheduler_HPP__ */

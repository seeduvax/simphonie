/*
 * @file ExInvalidSimulationTime.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_kern_ExInvalidSimulationTime_HPP__
#define __simphonie_kern_ExInvalidSimulationTime_HPP__
#include "Smp/Services/InvalidSimulationTime.h"
#include "simdeck/Exception.hpp"

namespace simphonie {
namespace kern {
using namespace simdeck;

/**
 *
 */
class ExInvalidSimulationTime : public Exception, virtual public Smp::Services::InvalidSimulationTime {
public:
    /**
     * Default constructor.
     */
    ExInvalidSimulationTime(Smp::IObject* sender, Smp::Duration cTime, Smp::Duration pTime, Smp::Duration mTime);
    /**
     * Destructor.
     */
    virtual ~ExInvalidSimulationTime();
    // Smp::Services::InvalidSimulationTime implementation
    Smp::Duration GetCurrentTime() const noexcept override;
    Smp::Duration GetProvidedTime() const noexcept override;
    Smp::Duration GetMaximumTime() const noexcept override;

private:
    Smp::Duration _cTime;
    Smp::Duration _pTime;
    Smp::Duration _mTime;
};

}  // namespace kern
}  // namespace simph
#endif  // __simphonie_kern_ExInvalidSimulationTime_HPP__

/*
 * @file ExInvalidSimulationTime.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_ExInvalidSimulationTime_HPP__
#define __simph_kern_ExInvalidSimulationTime_HPP__
#include "simph/smpdk/Exception.hpp"
#include "Smp/Services/InvalidSimulationTime.h"

namespace simph {
	namespace kern {
using namespace simph::smpdk;

/**
 *
 */
class ExInvalidSimulationTime: public Exception,
            virtual public Smp::Services::InvalidSimulationTime {
public:
    /**
     * Default constructor.
     */
    ExInvalidSimulationTime(Smp::IObject* sender,Smp::Duration cTime,
                        Smp::Duration pTime, Smp::Duration mTime);
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

}} // namespace simph::kern
#endif // __simph_kern_ExInvalidSimulationTime_HPP__

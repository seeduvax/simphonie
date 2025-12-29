/*
 * @file ExInvalidCycleTime.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_kern_ExInvalidCycleTime_HPP__
#define __simphonie_kern_ExInvalidCycleTime_HPP__

#include "Smp/Services/InvalidCycleTime.h"
#include "simdeck/Exception.hpp"

namespace simphonie {
namespace kern {

/**
 *
 */
class ExInvalidCycleTime: public simdeck::Exception, virtual public Smp::Services::InvalidCycleTime {
public:
    /**
     * Default constructor.
     */
    ExInvalidCycleTime(Smp::IObject* sender);
    /**
     * Destructor.
     */
    virtual ~ExInvalidCycleTime();

private:

};

}} // namespace simphonie::kern
#endif // __simphonie_kern_ExInvalidCycleTime_HPP__

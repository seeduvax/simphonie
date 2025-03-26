/*
 * @file ExInvalidType.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_ExInvalidType_HPP__
#define __simdeck_ExInvalidType_HPP__
#include "simdeck/Exception.hpp"
#include "Smp/InvalidType.h"

namespace simdeck {

/**
 *
 */
class ExInvalidType: public Exception, virtual public Smp::InvalidType {
public:
    /**
     * Default constructor.
     */
    ExInvalidType(const Smp::IObject* sender, Smp::String8 msg);
    /**
     * Destructor.
     */
    virtual ~ExInvalidType();

private:

};

} // namespace simdeck
#endif // __simdeck_ExInvalidType_HPP__

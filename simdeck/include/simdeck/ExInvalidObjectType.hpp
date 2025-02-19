/*
 * @file ExInvalidObjectType.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_ExInvalidObjectType_HPP__
#define __simdeck_ExInvalidObjectType_HPP__
#include "Smp/InvalidObjectType.h"
#include "simdeck/Exception.hpp"

namespace simdeck {

/**
 *
 */
class ExInvalidObjectType : public Exception, virtual public Smp::InvalidObjectType {
public:
    /**
     * Default constructor.
     */
    ExInvalidObjectType(const Smp::IObject* sender, Smp::IObject* invalid);
    /**
     * Destructor.
     */
    virtual ~ExInvalidObjectType();
    // Smp::InvalidObjectType exception
    Smp::IObject* GetInvalidObject() const noexcept;

private:
    Smp::IObject* _invalidObject;
};

}  // namespace simdeck
#endif  // __simdeck_ExInvalidObjectType_HPP__

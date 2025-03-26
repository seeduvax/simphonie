/*
 * @file ExInvalidObjectName.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_ExInvalidObjectName_HPP__
#define __simdeck_ExInvalidObjectName_HPP__
#include "simdeck/Exception.hpp"
#include "Smp/InvalidObjectName.h"

namespace simdeck {

/**
 *
 */
class ExInvalidObjectName : public Exception, virtual public Smp::InvalidObjectName {
public:
    /**
     * Default constructor.
     */
    ExInvalidObjectName(const Smp::IObject* sender, Smp::String8 invalidName);
    /**
     * Destructor.
     */
    virtual ~ExInvalidObjectName();
    // Smp::InvalidObjectName implementation
    Smp::String8 GetInvalidName() const noexcept override;

private:
    std::string _invalidName;
};

}  // namespace simdeck
#endif  // __simdeck_ExInvalidObjectName_HPP__

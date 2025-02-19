/*
 * @file ExInvalidArrayIndex.h
 *
 * Copyright 2020 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_ExInvalidArrayIndex_HPP__
#define __simdeck_ExInvalidArrayIndex_HPP__
#include "Smp/InvalidArrayIndex.h"
#include "simdeck/Exception.hpp"

namespace simdeck {
using namespace simdeck;

/**
 *
 */
class ExInvalidArrayIndex : public Exception, virtual public Smp::InvalidArrayIndex {
public:
    /**
     * Default constructor.
     */
    ExInvalidArrayIndex(const Smp::IObject* sender, Smp::Int64 invalid, Smp::Int64 size);
    /**
     * Destructor.
     */
    virtual ~ExInvalidArrayIndex();
    // Smp::InvalidArrayIndex implementation
    Smp::Int64 GetInvalidIndex() const noexcept override;
    Smp::Int64 GetArraySize() const noexcept override;

private:
    Smp::Int64 _invalidSize;
    Smp::Int64 _arraySize;
};

}  // namespace simdeck
#endif  // __simdeck_ExInvalidArrayIndex_HPP__

/*
 * @file ExInvalidParameterIndex.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_ExInvalidParameterIndex_HPP__
#define __simdeck_ExInvalidParameterIndex_HPP__
#include "simdeck/Exception.hpp"
#include "Smp/InvalidParameterIndex.h"

namespace simdeck {
using namespace simdeck;

/**
 *
 */
class ExInvalidParameterIndex : public Exception, virtual public Smp::InvalidParameterIndex {
public:
    /**
     * Default constructor.
     */
    ExInvalidParameterIndex(const Smp::IObject* sender, Smp::String8 opName, Smp::Int32 index, Smp::Int32 count);
    /**
     * Destructor.
     */
    virtual ~ExInvalidParameterIndex();
    // Smp::InvalidParameterIndex implementation
    Smp::String8 GetOperationName() const noexcept override;
    Smp::Int32 GetParameterIndex() const noexcept override;
    Smp::Int32 GetParameterCount() const noexcept override;

private:
    const Smp::String8 _opName;
    Smp::Int32 _index;
    Smp::Int32 _count;
};

}  // namespace simdeck
#endif  // __simdeck_ExInvalidParameterIndex_HPP__

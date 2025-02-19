/*
 * @file ExVoidOperation.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_ExVoidOperation_HPP__
#define __simdeck_ExVoidOperation_HPP__
#include "Smp/VoidOperation.h"
#include "simdeck/Exception.hpp"

namespace simdeck {
using namespace simdeck;

/**
 *
 */
class ExVoidOperation : public Exception, virtual public Smp::VoidOperation {
public:
    /**
     * Default constructor.
     */
    ExVoidOperation(const Smp::IObject* sender, Smp::String8 opName);
    /**
     * Destructor.
     */
    virtual ~ExVoidOperation();
    // Smp::VoidOperation implementation
    Smp::String8 GetOperationName() const noexcept override;

private:
    Smp::String8 _opName;
};

}  // namespace simdeck
#endif  // __simdeck_ExVoidOperation_HPP__

/*
 * @file ExDuplicateLiteral.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_ExDuplicateLiteral_HPP__
#define __simdeck_ExDuplicateLiteral_HPP__
#include "simdeck/Exception.hpp"
#include "Smp/Publication/DuplicateLiteral.h"

namespace simdeck {
using namespace simdeck;

/**
 *
 */
class ExDuplicateLiteral : public Exception, virtual public Smp::Publication::DuplicateLiteral {
public:
    /**
     * Default constructor.
     */
    ExDuplicateLiteral(const Smp::IObject* sender, Smp::String8 name, Smp::Int32 value);
    /**
     * Destructor.
     */
    virtual ~ExDuplicateLiteral();

    // Smp::Publication::DuplicateLiteral implementation
    Smp::String8 GetLiteralName() const noexcept override;
    Smp::Int32 GetLiteralValue() const noexcept override;

private:
    Smp::String8 _lName;
    Smp::Int32 _lValue;
};

}  // namespace simdeck
#endif  // __simdeck_ExDuplicateLiteral_HPP__

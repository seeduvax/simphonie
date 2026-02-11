/*
 * @file IUnit.hpp
 *
 * Copyright 2026 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_smpext_IUnit_HPP__
#define __simdeck_smpext_IUnit_HPP__

#include "Smp/IObject.h"

namespace simdeck {
namespace smpext {

/**
 *
 */
class IUnit: public virtual Smp::IObject {
public:
    /// Virtual desctructor to release memory
    virtual ~IUnit() noexcept = default;

    /**
     * @return unit symbol as a string
     */ 
    virtual Smp::String8 GetSymbol() const = 0;
};

}} // namespace simdeck::smpext
#endif // __simdeck_smpext_IUnit_HPP__

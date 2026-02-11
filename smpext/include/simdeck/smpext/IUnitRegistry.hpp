/*
 * @file IUnitRegistry.hpp
 *
 * Copyright 2026 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_smpext_IUnitRegistry_HPP__
#define __simdeck_smpext_IUnitRegistry_HPP__

#include "Smp/IService.h"
#include "simdeck/smpext/UnitCollection.hpp"

namespace simdeck {
namespace smpext {

/**
 *
 */
class IUnitRegistry: public virtual Smp::IService {
public:
    /// Virtual desctructor to release memory
    virtual ~IUnitRegistry() noexcept = default;

    /**
     * @return registered units collection
     */
    virtual const UnitCollection* GetUnits() const =0;
    /**
     * Search a unit by symbol inside this registry.
     * @return The found unit or nullptr if none.
     */
    virtual IUnit* GetUnit(Smp::String8 symbol) const = 0;
    /**
     * Add a new unit into this registry.
     * @param name unit name.
     * @param description unit detailed description.
     * @param symbol unit symbol.
     */ 
    virtual void AddUnit(Smp::String8 name, Smp::String8 description, Smp::String8 symbol) = 0;
};

}} // namespace simdeck::smpext
#endif // __simdeck_smpext_IUnitRegistry_HPP__

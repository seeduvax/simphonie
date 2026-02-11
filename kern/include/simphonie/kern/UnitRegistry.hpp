/*
 * @file UnitRegistry.hpp
 *
 * Copyright 2026 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_kern_UnitRegistry_HPP__
#define __simphonie_kern_UnitRegistry_HPP__

#include "simdeck/Service.hpp"
#include "simdeck/smpext/IUnitRegistry.hpp"
#include "simdeck/Collection.hpp"

namespace simphonie {
namespace kern {

/**
 *
 */
class UnitRegistry: public simdeck::Service, public virtual simdeck::smpext::IUnitRegistry {
    typedef simdeck::Service Base; 
public:
    /**
     * Default constructor.
     */
    UnitRegistry(Smp::String8 name, Smp::String8 description, Smp::IObject* parent);
    /**
     * Destructor.
     */
    virtual ~UnitRegistry();

    // IUnitRegistry implementation
    const simdeck::smpext::UnitCollection* GetUnits() const override;
    simdeck::smpext::IUnit* GetUnit(Smp::String8 symbol) const override;
    void AddUnit(Smp::String8 name, Smp::String8 description, Smp::String8 symbol) override;

private:
    simdeck::OwnedCollection<simdeck::smpext::IUnit> _units;

};

}} // namespace simphonie::kern
#endif // __simphonie_kern_UnitRegistry_HPP__

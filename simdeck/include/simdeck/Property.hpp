/*
 * @file Property.hpp
 *
 * Copyright 2020 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_Property_HPP__
#define __simdeck_Property_HPP__

#include "simdeck/Object.hpp"
#include "Smp/IProperty.h"

namespace simdeck {

/**
 *
 */
class Property : public simdeck::Object, virtual public Smp::IProperty {
public:
    /**
     * Default constructor.
     */
    Property(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent, Smp::Publication::IType* type,
             Smp::AccessKind access, Smp::ViewKind view);
    /**
     * Destructor.
     */
    virtual ~Property();

    // Smp::IProperty implementation
    Smp::Publication::IType* GetType() const override;
    Smp::AccessKind GetAccess() const override;
    Smp::ViewKind GetView() const override;
    Smp::AnySimple GetValue() const override;
    void SetValue(Smp::AnySimple value) override;
    Smp::PrimitiveTypeKind GetPrimitiveTypeKind() const override;

private:
    Smp::Publication::IType* _type;
    Smp::AccessKind _access;
    Smp::ViewKind _view;
    Smp::AnySimple _value;
};

}  // namespace simdeck
#endif  // __simdeck_Property_HPP__

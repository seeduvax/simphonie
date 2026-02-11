/*
 * @file SimpleType.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_SimpleType_HPP__
#define __simdeck_SimpleType_HPP__

#include "simdeck/SimpleField.hpp"
#include "Smp/IComponent.h"

namespace simdeck {

/**
 *
 */
class SimpleType: public Type {
public:
    SimpleType(Smp::Uuid uuid, Smp::PrimitiveTypeKind ptk, Smp::String8 name, Smp::String8 descr="", Smp::IObject* parent=nullptr);
    virtual ~SimpleType();
    // Smp::IObject specialization.
    Smp::IObject* GetChild(Smp::String8 name) const override;
    // specific services.
    inline void setUnit(Smp::IObject* unit) {
        _unit=unit;
    }
    void setMin(Smp::AnySimple min);
    void setMax(Smp::AnySimple max);
protected:
    Smp::IField* createField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::IComponent* parent,
        void* address,
        Smp::ViewKind view,
        Smp::Bool state,
        Smp::Bool input,
        Smp::Bool output) const override {
            return SimpleField::Create(name, description, view, this, address, state, input, output, parent);
    }
private:
    Smp::IObject* _unit=nullptr;
    Smp::ISimpleField* _min=nullptr;
    Smp::AnySimple _minValue;
    Smp::ISimpleField* _max=nullptr;
    Smp::AnySimple _maxValue;
};

} // namespace simdeck
#endif // __simdeck_SimpleType_HPP__

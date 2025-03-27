/*
 * @file StringField.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_StringField_HPP__
#define __simdeck_StringField_HPP__

#include "simdeck/Field.hpp"
#include "Smp/IForcibleField.h"
#include "Smp/ISimpleField.h"

namespace simdeck {

/**
 *
 */
class StringField: public Field, virtual public Smp::IForcibleField, virtual public Smp::ISimpleField {
    typedef Field Parent;
public:
    /**
     * Default constructor.
     */
    StringField(Smp::String8 name,
                Smp::String8 description,
                Smp::ViewKind viewKind,
                std::string* address,
                Smp::Publication::IType* type,
                Smp::Bool isState,
                Smp::Bool isInput,
                Smp::Bool isOutput,
                Smp::IObject* parent);
    /**
     * Destructor.
     */
    virtual ~StringField();

    // Smp::ISimpleField implementation
    Smp::PrimitiveTypeKind GetPrimitiveTypeKind() const override;
    Smp::AnySimple GetValue() const override;
    void SetValue(Smp::AnySimple value) override;

    // Smp::IForcibleField implementation
    void Force(Smp::AnySimple value) override;
    void Unforce() override;
    Smp::Bool IsForced() override;
    void Freeze() override;

    static StringField* Create(
                Smp::String8 name,
                Smp::String8 description,
                Smp::ViewKind viewKind,
                std::string* address,
                Smp::Publication::IType* type,
                Smp::Bool isState,
                Smp::Bool isInput,
                Smp::Bool isOutput,
                Smp::IObject* parent);

private:
    std::string* _value;
    std::string _forcedValue="";
    Smp::Bool _forced=false;
};

} // namespace simdeck
#endif // __simdeck_StringField_HPP__

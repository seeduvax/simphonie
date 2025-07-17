/*
 * @file FixedStringField.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_FixedStringField_HPP__
#define __simdeck_FixedStringField_HPP__

#include "simdeck/SimpleField.hpp"
#include "Smp/Publication/IStringType.h"

namespace simdeck {

/**
 *
 */
class FixedStringField: public SimpleField {
    typedef SimpleField Parent;
public:
    static Smp::IField* Create(Smp::String8 name, Smp::String8 description, Smp::ViewKind viewKind, char*  address, Smp::Bool isState,
           Smp::Bool isInput, Smp::Bool isOutput, Smp::IObject* parent, const Smp::Publication::IStringType* type);
    /**
     * Destructor.
     */
    virtual ~FixedStringField();

    // Smp::ISimpleField implementation
    Smp::AnySimple GetValue() const override;
    void SetValue(Smp::AnySimple value) override;
    void Force(Smp::AnySimple value) override;
    void Unforce() override;
    Smp::Bool IsForced() const override;
    void Freeze() override;
    void Restore(Smp::IStorageReader* reader) override;
    void Store(Smp::IStorageWriter* writer) override;    

protected:
    /**
     * Default constructor.
     */
    FixedStringField(Smp::String8 name, Smp::String8 description, Smp::ViewKind viewKind, void* address, Smp::Bool isState,
           Smp::Bool isInput, Smp::Bool isOutput, Smp::IObject* parent, const Smp::Publication::IStringType* type);

private:
    char* _value;
    char* _forcedValue;
    Smp::Bool _isForced;
    Smp::UInt64 _maxLength;
};

} // namespace simdeck
#endif // __simdeck_FixedStringField_HPP__

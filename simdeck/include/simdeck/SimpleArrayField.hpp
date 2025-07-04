/*
 * @file SimpleArrayField.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_SimpleArrayField_HPP__
#define __simdeck_SimpleArrayField_HPP__

#include "simdeck/Field.hpp"
#include "Smp/ISimpleArrayField.h"

namespace simdeck {

/**
 * Smp::ISimpleArrayField implementation
 */ 
class SimpleArrayField : public Field, public virtual Smp::ISimpleArrayField {
public:
    template <typename T>
    static Smp::ISimpleArrayField* Create(
                      Smp::String8 name, Smp::String8 description,
                      Smp::UInt64 count, T* address,
                      const Smp::Publication::IType* ptype, Smp::ViewKind viewKind,
                      const Smp::Publication::IType* type,
                      Smp::Bool isState, Smp::Bool isInput, Smp::Bool isOutput,
                      Smp::IObject* parent);
    static Smp::ISimpleArrayField* Create(
                      Smp::String8 name, Smp::String8 description,
                      Smp::UInt64 count, void* address,
                      const Smp::Publication::IType* ptype, Smp::ViewKind viewKind,
                      const Smp::Publication::IType* type,
                      Smp::Bool isState, Smp::Bool isInput, Smp::Bool isOutput,
                      Smp::IObject* parent);
    virtual ~SimpleArrayField();
protected:
    SimpleArrayField(Smp::String8 name, Smp::String8 description,
                      Smp::UInt64 count, void* address, Smp::UInt64 itemSize,
                      Smp::ViewKind viewKind,
                      const Smp::Publication::IType* type,
                      Smp::Bool isState, Smp::Bool isInput, Smp::Bool isOutput,
                      Smp::IObject* parent);
};

std::ostream& toprint(std::ostream& os, const Smp::ISimpleArrayField& obj);

} // namespace simdeck
#endif // __simdeck_SimpleArrayField_HPP__

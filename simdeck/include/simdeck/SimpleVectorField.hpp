/*
 * @file SimpleVectorField.hpp
 *
 * Copyright 2026 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_SimpleVectorField_HPP__
#define __simdeck_SimpleVectorField_HPP__

#include "simdeck/Field.hpp"
#include "Smp/ISimpleArrayField.h"

namespace simdeck {
/**
 * Smp::ISimpleArrayField implementation
 */ 
class SimpleVectorField : public Field, public virtual Smp::ISimpleArrayField {
public:
   
    static Smp::ISimpleArrayField* Create(
                      Smp::String8 name, Smp::String8 description,
                      void* address,
                      const Smp::Publication::IType* ptype, Smp::ViewKind viewKind,
                      const Smp::Publication::IType* type,
                      Smp::Bool isState, Smp::Bool isInput, Smp::Bool isOutput,
                      Smp::IObject* parent);
    virtual ~SimpleVectorField();
protected:
    SimpleVectorField(Smp::String8 name, Smp::String8 description,
                      void* address, 
                      Smp::ViewKind viewKind,
                      const Smp::Publication::IType* type,
                      Smp::Bool isState, Smp::Bool isInput, Smp::Bool isOutput,
                      Smp::IObject* parent);
private: 
    template <typename T>
    static Smp::ISimpleArrayField* Create(
                      Smp::String8 name, Smp::String8 description,
                      std::vector<T>* address,
                      const Smp::Publication::IType* ptype, Smp::ViewKind viewKind,
                      const Smp::Publication::IType* type,
                      Smp::Bool isState, Smp::Bool isInput, Smp::Bool isOutput,
                      Smp::IObject* parent);
};

} // namespace simdeck
#endif // __simdeck_SimpleVectorField_HPP__

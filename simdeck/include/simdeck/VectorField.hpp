/*
 * @file VectorField.hpp
 *
 * Copyright 2026 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_VectorField_HPP__
#define __simdeck_VectorField_HPP__

#include "Smp/IArrayField.h"
#include "simdeck/Field.hpp"

namespace simdeck {

/**
 *
 */
class VectorField : public Field, public virtual Smp::IArrayField {
public:
  
    /**
     * Destructor.
     */
    virtual ~VectorField();

  

    static Smp::IArrayField* Create(Smp::String8 name, Smp::String8 description,
                      void* address,
                      Smp::Publication::IType* ptype, Smp::ViewKind viewKind,
                      const Smp::Publication::IType* type,
                      Smp::Bool isState, Smp::Bool isInput, Smp::Bool isOutput,
                      Smp::IObject* parent);
                      

protected:
    VectorField(Smp::String8 name, Smp::String8 description,
                      void* address,
                      Smp::ViewKind viewKind,
                      const Smp::Publication::IType* type,
                      Smp::Bool isState, Smp::Bool isInput, Smp::Bool isOutput,
                      Smp::IObject* parent);
private:
    template <typename T>
    static Smp::IArrayField* Create(
                            Smp::String8 name,
                            Smp::String8 description,
                            std::vector<T*>* address,
                            Smp::Publication::IType* ptype,
                            Smp::ViewKind viewKind,
                            const Smp::Publication::IType* type,
                            Smp::Bool isState,
                            Smp::Bool isInput,
                            Smp::Bool isOutput,
                            Smp::IObject* parent);                      
};

} // namespace simdeck
#endif // __simdeck_VectorField_HPP__
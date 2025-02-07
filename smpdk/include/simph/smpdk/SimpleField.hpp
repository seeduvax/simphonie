/*
 * @file SimpleField.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_smpdk_SimpleField_HPP__
#define __simph_smpdk_SimpleField_HPP__

#include "Smp/ISimpleField.h"
#include "simph/smpdk/Field.hpp"

namespace simph {
namespace smpdk {

class SimpleField : public Field, virtual public Smp::IForcibleField {
public:
    /**
     * Destructor.
     */
    virtual ~SimpleField();
    template <typename T>
    static Smp::ISimpleField* Create(  Smp::String8 name,
                            Smp::String8 description,
                            Smp::ViewKind viewKind,
                            T* address,
                            Smp::Bool isState,
                            Smp::Bool isInput,
                            Smp::Bool isOutput,
                            Smp::IObject* parent);
    static Smp::ISimpleField* Create(  Smp::String8 name,
                            Smp::String8 description,
                            Smp::ViewKind viewKind,
                            void* address,
                            Smp::Bool isState,
                            Smp::Bool isInput,
                            Smp::Bool isOutput,
                            Smp::IObject* parent,
                            Smp::Uuid typeUuid);
    Smp::PrimitiveTypeKind GetPrimitiveTypeKind() const override;
protected:
    SimpleField(Smp::String8 name, Smp::String8 description,
                Smp::ViewKind viewKind, void* address, unsigned int dataSize,
                Smp::Publication::IType* type, Smp::Bool isState,
                Smp::Bool isInput, Smp::Bool isOutput,
                Smp::IObject* parent);

};

std::ostream& toprint(std::ostream& os, const Smp::ISimpleField& obj);

}} // namespace simph::smpdk
#endif // __simph_smpdk_SimpleField_HPP__

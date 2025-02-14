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
    static Smp::ISimpleField* Create(  Smp::String8 name,
                            Smp::String8 description,
                            Smp::ViewKind viewKind,
                            void* address,
                            Smp::Bool isState,
                            Smp::Bool isInput,
                            Smp::Bool isOutput,
                            Smp::IObject* parent,
                            Smp::Uuid typeUuid);
    /**
     * Destructor.
     */
    virtual ~SimpleField();
    Smp::PrimitiveTypeKind GetPrimitiveTypeKind() const override;

    template <typename T>
    static Smp::Uuid GetPrimitiveUuid() { return Smp::Uuid(); }

protected:
    SimpleField(Smp::String8 name, Smp::String8 description,
                Smp::ViewKind viewKind, void* address, unsigned int dataSize,
                Smp::Publication::IType* type, Smp::Bool isState,
                Smp::Bool isInput, Smp::Bool isOutput,
                Smp::IObject* parent);
private:
    template <typename T>
    static Smp::ISimpleField* Create(  Smp::String8 name,
                            Smp::String8 description,
                            Smp::ViewKind viewKind,
                            T* address,
                            Smp::Bool isState,
                            Smp::Bool isInput,
                            Smp::Bool isOutput,
                            Smp::IObject* parent);
};

std::ostream& toprint(std::ostream& os, const Smp::ISimpleField& obj);

template <>
Smp::Uuid SimpleField::GetPrimitiveUuid<Smp::Bool>();
template <>
Smp::Uuid SimpleField::GetPrimitiveUuid<Smp::Char8>();
template <>
Smp::Uuid SimpleField::GetPrimitiveUuid<Smp::Int8>();
template <>
Smp::Uuid SimpleField::GetPrimitiveUuid<Smp::Int16>();
template <>
Smp::Uuid SimpleField::GetPrimitiveUuid<Smp::Int32>();
template <>
Smp::Uuid SimpleField::GetPrimitiveUuid<Smp::Int64>();
template <>
Smp::Uuid SimpleField::GetPrimitiveUuid<Smp::UInt8>();
template <>
Smp::Uuid SimpleField::GetPrimitiveUuid<Smp::UInt16>();
template <>
Smp::Uuid SimpleField::GetPrimitiveUuid<Smp::UInt32>();
template <>
Smp::Uuid SimpleField::GetPrimitiveUuid<Smp::UInt64>();
template <>
Smp::Uuid SimpleField::GetPrimitiveUuid<Smp::Float32>();
template <>
Smp::Uuid SimpleField::GetPrimitiveUuid<Smp::Float64>();

}} // namespace simph::smpdk
#endif // __simph_smpdk_SimpleField_HPP__

/*
 * @file SimpleField.cpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simdeck/SimpleField.hpp"
#include "simdeck/Collection.hpp"
#include "Smp/IOutputField.h"
#include "simdeck/ExInvalidTarget.hpp"


namespace simdeck {
template <typename T>
class TSimpleField : public SimpleField, virtual public Smp::ISimpleField {
public:
    TSimpleField(Smp::String8 name, Smp::String8 description, Smp::ViewKind viewKind, T* address, Smp::Bool isState,
           Smp::Bool isInput, Smp::Bool isOutput, Smp::IObject* parent)
        : SimpleField(name, description, viewKind, (void*)address, sizeof(T), nullptr, isState, isInput, isOutput, parent),
          _tData(address) {
        initType();
    }
    virtual ~TSimpleField() {}
    // Smp::ISimpleField implementation
    Smp::PrimitiveTypeKind GetPrimitiveTypeKind() const override;
    Smp::AnySimple GetValue() const override;

    void SetValue(Smp::AnySimple value) override {
        *_tData = value;
    }
    // Smp::IForcibleField implementation
    void Force(Smp::AnySimple value) override {
        _forcedValue=value;
        _forced=true;
    }
    void Unforce() override {
        _forced=false;
    }
    Smp::Bool IsForced() const override {
        return _forced;
    }
    void Freeze() override {
        _forcedValue=*_tData;
        _forced=true;
    }
protected:
    void initType();

private:
    T* _tData;
    T _forcedValue{0};
    Smp::Bool _forced{false};
};
template <typename T>
class TSimpleOutputField : public TSimpleField<T>, virtual public Smp::IOutputField {
public:
    TSimpleOutputField(Smp::String8 name,
                Smp::String8 description,
                Smp::ViewKind viewKind,
                T* address,
                Smp::Bool isState,
                Smp::Bool isInput,
                Smp::IObject* parent
                ):  TSimpleField<T>(name, description, viewKind, address,
                         isState, false, true, parent),
                    _targets("targets","connected fields",this) {
    } 
    virtual ~TSimpleOutputField() {
    }

    // Smp::IOutputField implementation
    void Connect(Smp::IField* target) override {
        auto sf=dynamic_cast<Smp::ISimpleField*>(target);
        if (sf!=nullptr && !_targets.contain(sf)) {
            _targets.push_back(sf);
        }
        if (sf==nullptr) {
            throw ExInvalidTarget(this,target);
        }
    }
    void Disconnect(Smp::IField* target) override {
        auto sf=dynamic_cast<Smp::ISimpleField*>(target);
        if (sf!=nullptr && _targets.contain(sf)) {
            _targets.remove(sf);
        }
    }
    void Push() override {
        for (auto target: _targets) {
            // TODO should not need to cast anything here, find a way to have
            // a fully resolved collection type while being able to return
            // properly the field collection whith GetInputFields 
            auto sf=dynamic_cast<Smp::ISimpleField*>(target);
            if (sf!=nullptr) {
                sf->SetValue(this->GetValue());
            }
        }
    }
    const Smp::FieldCollection* GetInputFields() const override {
        return &_targets;
    }
    Smp::Bool IsAutomatic() const {
        // The runtime and scheduler shall handle data propagation when needed,
        // i.e when the field owner changed. This should be when one of its
        // entry point is executed.
        // So since, the Push is not called on SetValue(), it is considered
        // here not being automatic. Rational of this decision: Field owner
        // would probably update the data directly without using SetValue since
        // it onws the data wrapped in this field.
        return false;
    }
private:
    Collection<Smp::IField> _targets;
    
};

template <typename T>
Smp::ISimpleField* SimpleField::Create(
                            Smp::String8 name,
                            Smp::String8 description,
                            Smp::ViewKind viewKind,
                            T* address,
                            Smp::Bool isState,
                            Smp::Bool isInput,
                            Smp::Bool isOutput,
                            Smp::IObject* parent) {
    if (isOutput) {
        return new TSimpleOutputField<T>(
                        name, description, viewKind, static_cast<T*>(address),
                        isState, isInput, parent);
    }
    else {
        return new TSimpleField<T>(
                        name, description, viewKind, static_cast<T*>(address),
                        isState, isInput, false, parent);
    }
}
// --------------------------------------------------------------------
// ..........................................................
SimpleField::SimpleField(
             Smp::String8 name, Smp::String8 description,
             Smp::ViewKind viewKind, void* address, unsigned int dataSize,
             Smp::Publication::IType* type, Smp::Bool isState,
             Smp::Bool isInput, Smp::Bool isOutput,
             Smp::IObject* parent)
            : Field(name, description, viewKind, address, dataSize, type,
                    isState, isInput, isOutput, parent) {
}
// ..........................................................
SimpleField::~SimpleField() {
}
// ..........................................................
Smp::ISimpleField* SimpleField::Create(
                            Smp::String8 name,
                            Smp::String8 description,
                            Smp::ViewKind viewKind,
                            void* address,
                            Smp::Bool isState,
                            Smp::Bool isInput,
                            Smp::Bool isOutput,
                            Smp::IObject* parent,
                            Smp::Uuid typeUuid) {
    if (typeUuid == Smp::Uuids::Uuid_Bool) {
        auto f=SimpleField::Create(name, description, viewKind, 
                                    (Smp::Bool*)address,
                                    isState, isInput, isOutput,
                                    parent);
        return f;
    }
    else if (typeUuid == Smp::Uuids::Uuid_Char8) {
        auto f=SimpleField::Create(name, description, viewKind,
                                    (Smp::Char8*)address,
                                    isState, isInput, isOutput,
                                    parent);
        return f;
    }
    else if (typeUuid == Smp::Uuids::Uuid_Int8) {
        auto f=SimpleField::Create(name, description, viewKind,
                                    (Smp::Int8*)address,
                                    isState, isInput, isOutput,
                                    parent);
        return f;
    }
    else if (typeUuid == Smp::Uuids::Uuid_Int16) {
        auto f=SimpleField::Create(name, description, viewKind,
                                    (Smp::Int16*)address,
                                    isState, isInput, isOutput,
                                    parent);
        return f;
    }
    else if (typeUuid == Smp::Uuids::Uuid_Int32) {
        auto f=SimpleField::Create(name, description, viewKind,
                                    (Smp::Int32*)address,
                                    isState, isInput, isOutput,
                                    parent);
        return f;
    }
    else if (typeUuid == Smp::Uuids::Uuid_Int64) {
        auto f=SimpleField::Create(name, description, viewKind,
                                    (Smp::Int64*)address,
                                    isState, isInput, isOutput,
                                    parent);
        return f;
    }
    else if (typeUuid == Smp::Uuids::Uuid_UInt8) {
        auto f=SimpleField::Create(name, description, viewKind,
                                    (Smp::UInt8*)address,
                                    isState, isInput, isOutput,
                                    parent);
        return f;
    }
    else if (typeUuid == Smp::Uuids::Uuid_UInt16) {
        auto f=SimpleField::Create(name, description, viewKind,
                                    (Smp::UInt16*)address,
                                    isState, isInput, isOutput,
                                    parent);
        return f;
    }
    else if (typeUuid == Smp::Uuids::Uuid_UInt32) {
        auto f=SimpleField::Create(name, description, viewKind,
                                    (Smp::UInt32*)address,
                                    isState, isInput, isOutput,
                                    parent);
        return f;
    }
    else if (typeUuid == Smp::Uuids::Uuid_UInt64) {
        auto f=SimpleField::Create(name, description, viewKind,
                                    (Smp::UInt64*)address,
                                    isState, isInput, isOutput,
                                    parent);
        return f;
    }
    else if (typeUuid == Smp::Uuids::Uuid_Float32) {
        auto f=SimpleField::Create(name, description, viewKind,
                                    (Smp::Float32*)address,
                                    isState, isInput, isOutput,
                                    parent);
        return f;
    }
    else if (typeUuid == Smp::Uuids::Uuid_Float64) {
        auto f=SimpleField::Create(name, description, viewKind,
                                   (Smp::Float64*)address,
                                   isState, isInput, isOutput,
                                   parent);
        return f;
    }
    return nullptr;
}
// ..........................................................
Smp::PrimitiveTypeKind SimpleField::GetPrimitiveTypeKind() const {
    return Smp::PrimitiveTypeKind::PTK_None;
}
// ..........................................................
template <>
Smp::PrimitiveTypeKind TSimpleField<Smp::Char8>::GetPrimitiveTypeKind() const {
    return Smp::PrimitiveTypeKind::PTK_Char8;
}
// ..........................................................
template <>
Smp::PrimitiveTypeKind TSimpleField<Smp::Bool>::GetPrimitiveTypeKind() const {
    return Smp::PrimitiveTypeKind::PTK_Bool;
}
// ..........................................................
template <>
Smp::PrimitiveTypeKind TSimpleField<Smp::Int8>::GetPrimitiveTypeKind() const {
    return Smp::PrimitiveTypeKind::PTK_Int8;
}
// ..........................................................
template <>
Smp::PrimitiveTypeKind TSimpleField<Smp::Int16>::GetPrimitiveTypeKind() const {
    return Smp::PrimitiveTypeKind::PTK_Int16;
}
// ..........................................................
template <>
Smp::PrimitiveTypeKind TSimpleField<Smp::Int32>::GetPrimitiveTypeKind() const {
    return Smp::PrimitiveTypeKind::PTK_Int32;
}
// ..........................................................
template <>
Smp::PrimitiveTypeKind TSimpleField<Smp::Int64>::GetPrimitiveTypeKind() const {
    return Smp::PrimitiveTypeKind::PTK_Int64;
}
// ..........................................................
template <>
Smp::PrimitiveTypeKind TSimpleField<Smp::UInt8>::GetPrimitiveTypeKind() const {
    return Smp::PrimitiveTypeKind::PTK_UInt8;
}
// ..........................................................
template <>
Smp::PrimitiveTypeKind TSimpleField<Smp::UInt16>::GetPrimitiveTypeKind() const {
    return Smp::PrimitiveTypeKind::PTK_UInt16;
}
// ..........................................................
template <>
Smp::PrimitiveTypeKind TSimpleField<Smp::UInt32>::GetPrimitiveTypeKind() const {
    return Smp::PrimitiveTypeKind::PTK_UInt32;
}
// ..........................................................
template <>
Smp::PrimitiveTypeKind TSimpleField<Smp::UInt64>::GetPrimitiveTypeKind() const {
    return Smp::PrimitiveTypeKind::PTK_UInt64;
}
// ..........................................................
template <>
Smp::PrimitiveTypeKind TSimpleField<Smp::Float32>::GetPrimitiveTypeKind() const {
    return Smp::PrimitiveTypeKind::PTK_Float32;
}
// ..........................................................
template <>
Smp::PrimitiveTypeKind TSimpleField<Smp::Float64>::GetPrimitiveTypeKind() const {
    return Smp::PrimitiveTypeKind::PTK_Float64;
}
// ..........................................................
template <>
Smp::AnySimple TSimpleField<Smp::Char8>::GetValue() const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Char8, _forced?_forcedValue:*_tData);
}
// ..........................................................
template <>
Smp::AnySimple TSimpleField<Smp::Bool>::GetValue() const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Bool, _forced?_forcedValue:*_tData);
}
// ..........................................................
template <>
Smp::AnySimple TSimpleField<Smp::Int8>::GetValue() const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Int8, _forced?_forcedValue:*_tData);
}
// ..........................................................
template <>
Smp::AnySimple TSimpleField<Smp::UInt8>::GetValue() const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_UInt8, _forced?_forcedValue:*_tData);
}
// ..........................................................
template <>
Smp::AnySimple TSimpleField<Smp::Int16>::GetValue() const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Int16, _forced?_forcedValue:*_tData);
}
// ..........................................................
template <>
Smp::AnySimple TSimpleField<Smp::UInt16>::GetValue() const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_UInt16, _forced?_forcedValue:*_tData);
}
// ..........................................................
template <>
Smp::AnySimple TSimpleField<Smp::Int32>::GetValue() const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Int32, _forced?_forcedValue:*_tData);
}
// ..........................................................
template <>
Smp::AnySimple TSimpleField<Smp::UInt32>::GetValue() const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_UInt32, _forced?_forcedValue:*_tData);
}
// ..........................................................
template <>
Smp::AnySimple TSimpleField<Smp::Int64>::GetValue() const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Int64, _forced?_forcedValue:*_tData);
}
// ..........................................................
template <>
Smp::AnySimple TSimpleField<Smp::UInt64>::GetValue() const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_UInt64, _forced?_forcedValue:*_tData);
}
// ..........................................................
template <>
Smp::AnySimple TSimpleField<Smp::Float32>::GetValue() const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Float32, _forced?_forcedValue:*_tData);
}
// ..........................................................
template <>
Smp::AnySimple TSimpleField<Smp::Float64>::GetValue() const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Float64, _forced?_forcedValue:*_tData);
}
// ..........................................................
template <>
Smp::AnySimple TSimpleField<Smp::String8>::GetValue() const {
    // TODO this is probably wrong and not the way strings should be handled.
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_String8, _forced?_forcedValue:*_tData);
}
// ..........................................................
// to print Smp::ISimpleField
std::ostream& toprint(std::ostream& os, const Smp::ISimpleField& obj) {
    os << obj.GetValue();
    return os;
}

// ..........................................................
template <>
void TSimpleField<Smp::Char8>::initType() {
    setType(&_char8Type);
}
template <>
void TSimpleField<Smp::Bool>::initType() {
    setType(&_boolType);
}
template <>
void TSimpleField<Smp::Int8>::initType() {
    setType(&_int8Type);
}
template <>
void TSimpleField<Smp::Int16>::initType() {
    setType(&_int16Type);
}
template <>
void TSimpleField<Smp::Int32>::initType() {
    setType(&_int32Type);
}
template <>
void TSimpleField<Smp::Int64>::initType() {
    setType(&_int64Type);
}
template <>
void TSimpleField<Smp::UInt8>::initType() {
    setType(&_uint8Type);
}
template <>
void TSimpleField<Smp::UInt16>::initType() {
    setType(&_uint16Type);
}
template <>
void TSimpleField<Smp::UInt32>::initType() {
    setType(&_uint32Type);
}
template <>
void TSimpleField<Smp::UInt64>::initType() {
    setType(&_uint64Type);
}
template <>
void TSimpleField<Smp::Float32>::initType() {
    setType(&_float32Type);
}
template <>
void TSimpleField<Smp::Float64>::initType() {
    setType(&_float64Type);
}

template <>
Smp::Uuid SimpleField::GetPrimitiveUuid<Smp::Bool>() { return Smp::Uuids::Uuid_Bool; }
template <>
Smp::Uuid SimpleField::GetPrimitiveUuid<Smp::Char8>() { return Smp::Uuids::Uuid_Char8; }
template <>
Smp::Uuid SimpleField::GetPrimitiveUuid<Smp::Int8>() { return Smp::Uuids::Uuid_Int8; }
template <>
Smp::Uuid SimpleField::GetPrimitiveUuid<Smp::Int16>() { return Smp::Uuids::Uuid_Int16; }
template <>
Smp::Uuid SimpleField::GetPrimitiveUuid<Smp::Int32>() { return Smp::Uuids::Uuid_Int32; }
template <>
Smp::Uuid SimpleField::GetPrimitiveUuid<Smp::Int64>() { return Smp::Uuids::Uuid_Int64; }
template <>
Smp::Uuid SimpleField::GetPrimitiveUuid<Smp::UInt8>() { return Smp::Uuids::Uuid_UInt8; }
template <>
Smp::Uuid SimpleField::GetPrimitiveUuid<Smp::UInt16>() { return Smp::Uuids::Uuid_UInt16; }
template <>
Smp::Uuid SimpleField::GetPrimitiveUuid<Smp::UInt32>() { return Smp::Uuids::Uuid_UInt32; }
template <>
Smp::Uuid SimpleField::GetPrimitiveUuid<Smp::UInt64>() { return Smp::Uuids::Uuid_UInt64; }
template <>
Smp::Uuid SimpleField::GetPrimitiveUuid<Smp::Float32>() { return Smp::Uuids::Uuid_Float32; }
template <>
Smp::Uuid SimpleField::GetPrimitiveUuid<Smp::Float64>() { return Smp::Uuids::Uuid_Float64; }
} // namespace simdeck

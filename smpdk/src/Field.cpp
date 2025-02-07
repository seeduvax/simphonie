/*
 * @file Field.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/smpdk/Field.hpp"
#include "simph/smpdk/ExInvalidTarget.hpp"
#include "simph/smpdk/StructureType.hpp"
#include "simph/smpdk/Type.hpp"

namespace simph {
namespace smpdk {
// --------------------------------------------------------------------
// ..........................................................
Field::Field(Smp::String8 name, Smp::String8 description,
             Smp::ViewKind viewKind, void* address, unsigned int dataSize,
             Smp::Publication::IType* type, Smp::Bool isState,
             Smp::Bool isInput, Smp::Bool isOutput,
             Smp::IObject* parent)
    : Persist(name, description, parent),
      _stateType(isState),
      _inputType(isInput),
      _outputType(isOutput),
      _type(type),
      _viewKind(viewKind),
      _data(address == nullptr ? malloc(dataSize) : address),
      _dataSize(dataSize),
      _allocated(address == nullptr) {
}
// ..........................................................
Field::~Field() {
    if (_allocated) {
        free(_data);
    }
}
// --------------------------------------------------------------------
// ..........................................................
Smp::ViewKind Field::GetView() const {
    return _viewKind;
}
// ..........................................................
Smp::Bool Field::IsState() const {
    return _stateType;
}
// ..........................................................
Smp::Bool Field::IsInput() const {
    return _inputType;
}
// ..........................................................
Smp::Bool Field::IsOutput() const {
    return _outputType;
}
// ..........................................................
const Smp::Publication::IType* Field::GetType() const {
    return _type;
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
Smp::IField* SimpleField::Create(
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

// --------------------------------------------------------------------
// StructureField
// ..........................................................
StructureField::StructureField(Smp::String8 name, Smp::String8 description, Smp::ViewKind viewKind, void* address,
                               Smp::Publication::IType* type, Smp::Bool isState, Smp::Bool isInput, Smp::Bool isOutput,
                               Smp::IObject* parent)
    : Field(name, description, viewKind, address, 0, type, isState, isInput, isOutput, parent) {
    auto st = dynamic_cast<StructureType*>(type);
    if (st != nullptr) {
        st->setup(this);
    }
    else {
// TODO        LOGE("Can't setup field " << name << ", its type " << type->GetName() << " is not an expected Structure Type.");
    }
}
// ..........................................................
StructureField::~StructureField() {}
// ..........................................................
/* TODO to be restored with OutputField
void StructureField::Push() {
    for (auto f : _fields) {
        f->Push();
    }
}
// ..........................................................
void StructureField::Connect(Smp::IField* target) {
    auto f = dynamic_cast<StructureField*>(target);
    if (f != nullptr && f->GetType()->GetUuid() == GetType()->GetUuid() && IsOutput() && f->IsInput()
        && _fields.size() == f->_fields.size()) {
        for (int i = 0; i < _fields.size(); i++) {
            _fields[i]->Connect(f->_fields[i]);
        }
    }
    else {
        throw ExInvalidTarget(this, target);
    }
}
*/
// ..........................................................
void StructureField::addField(Smp::IField* f) {
    _fields.push_back(f);
}
// --------------------------------------------------------------------
// Array Field
// ..........................................................
SimpleArrayField::SimpleArrayField(
                      Smp::String8 name, Smp::String8 description,
                      Smp::UInt64 count, void* address, Smp::UInt64 itemSize,
                      Smp::ViewKind viewKind,
                      Smp::Publication::IType* type,
                      Smp::Bool isState, Smp::Bool isInput, Smp::Bool isOutput,
                      Smp::IObject* parent)
                    : Field(name, description, viewKind, address, 
                            itemSize * count, type, isState, isInput,
                            isOutput, parent) {
}
// ..........................................................
SimpleArrayField::~SimpleArrayField() {
}
// --------------------------------------------------------------------
// ..........................................................
template <>
Smp::AnySimple TSimpleArrayField<Smp::Char8>::GetValue(Smp::UInt64 index) const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Char8, _tData[index]);
}
// ..........................................................
template <>
Smp::AnySimple TSimpleArrayField<Smp::Bool>::GetValue(Smp::UInt64 index) const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Bool, _tData[index]);
}
// ..........................................................
template <>
Smp::AnySimple TSimpleArrayField<Smp::Int8>::GetValue(Smp::UInt64 index) const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Int8, _tData[index]);
}
// ..........................................................
template <>
Smp::AnySimple TSimpleArrayField<Smp::Int16>::GetValue(Smp::UInt64 index) const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Int16, _tData[index]);
}
// ..........................................................
template <>
Smp::AnySimple TSimpleArrayField<Smp::Int32>::GetValue(Smp::UInt64 index) const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Int32, _tData[index]);
}
// ..........................................................
template <>
Smp::AnySimple TSimpleArrayField<Smp::Int64>::GetValue(Smp::UInt64 index) const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Int64, _tData[index]);
}
// ..........................................................
template <>
Smp::AnySimple TSimpleArrayField<Smp::UInt8>::GetValue(Smp::UInt64 index) const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_UInt8, _tData[index]);
}
// ..........................................................
template <>
Smp::AnySimple TSimpleArrayField<Smp::UInt16>::GetValue(Smp::UInt64 index) const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_UInt16, _tData[index]);
}
// ..........................................................
template <>
Smp::AnySimple TSimpleArrayField<Smp::UInt32>::GetValue(Smp::UInt64 index) const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_UInt32, _tData[index]);
}
// ..........................................................
template <>
Smp::AnySimple TSimpleArrayField<Smp::UInt64>::GetValue(Smp::UInt64 index) const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_UInt64, _tData[index]);
}
// ..........................................................
template <>
Smp::AnySimple TSimpleArrayField<Smp::Float32>::GetValue(Smp::UInt64 index) const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Float32, _tData[index]);
}
// ..........................................................
template <>
Smp::AnySimple TSimpleArrayField<Smp::Float64>::GetValue(Smp::UInt64 index) const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Float64, _tData[index]);
}
// ..........................................................
template <typename T>
Smp::ISimpleArrayField* SimpleArrayField::Create(
                            Smp::String8 name,
                            Smp::String8 description,
                            Smp::UInt64 count,
                            T* address,
                            Smp::PrimitiveTypeKind ptype,
                            Smp::ViewKind viewKind,
                            Smp::Publication::IType* type,
                            Smp::Bool isState,
                            Smp::Bool isInput,
                            Smp::Bool isOutput,
                            Smp::IObject* parent) {
    if (isOutput) {
        return new TSimpleArrayOutputField<T>(
                        name, description, count,
                        static_cast<T*>(address), ptype, viewKind, type,
                        isState, isInput, parent);
    }
    else {
        return new TSimpleArrayField<T>(
                        name, description, count,
                        static_cast<T*>(address), ptype, viewKind, type,
                        isState, isInput, false, parent);
    }
}
// ..........................................................
Smp::ISimpleArrayField* SimpleArrayField::Create(
                            Smp::String8 name,
                            Smp::String8 description,
                            Smp::UInt64 count,
                            void* address,
                            Smp::PrimitiveTypeKind ptype,
                            Smp::ViewKind viewKind,
                            Smp::Publication::IType* type,
                            Smp::Bool isState,
                            Smp::Bool isInput,
                            Smp::Bool isOutput,
                            Smp::IObject* parent) {
    if (ptype == Smp::PrimitiveTypeKind::PTK_Bool) {
        auto f=SimpleArrayField::Create(name, description, count, 
                                    (Smp::Bool*)address,
                                    ptype, viewKind, type,
                                    isState, isInput, isOutput,
                                    parent);
        return f;
    }
    else if (ptype == Smp::PrimitiveTypeKind::PTK_Char8) {
        auto f=SimpleArrayField::Create(name, description, count,
                                    (Smp::Char8*)address,
                                    ptype, viewKind, type,
                                    isState, isInput, isOutput,
                                    parent);
        return f;
    }
    else if (ptype == Smp::PrimitiveTypeKind::PTK_Int8) {
        auto f=SimpleArrayField::Create(name, description, count,
                                    (Smp::Int8*)address,
                                    ptype, viewKind, type,
                                    isState, isInput, isOutput,
                                    parent);
        return f;
    }
    else if (ptype == Smp::PrimitiveTypeKind::PTK_Int16) {
        auto f=SimpleArrayField::Create(name, description, count,
                                    (Smp::Int16*)address,
                                    ptype, viewKind, type,
                                    isState, isInput, isOutput,
                                    parent);
        return f;
    }
    else if (ptype == Smp::PrimitiveTypeKind::PTK_Int32) {
        auto f=SimpleArrayField::Create(name, description, count,
                                    (Smp::Int32*)address,
                                    ptype, viewKind, type,
                                    isState, isInput, isOutput,
                                    parent);
        return f;
    }
    else if (ptype == Smp::PrimitiveTypeKind::PTK_Int64) {
        auto f=SimpleArrayField::Create(name, description, count,
                                    (Smp::Int64*)address,
                                    ptype, viewKind, type,
                                    isState, isInput, isOutput,
                                    parent);
        return f;
    }
    else if (ptype == Smp::PrimitiveTypeKind::PTK_UInt8) {
        auto f=SimpleArrayField::Create(name, description, count,
                                    (Smp::UInt8*)address,
                                    ptype, viewKind, type,
                                    isState, isInput, isOutput,
                                    parent);
        return f;
    }
    else if (ptype == Smp::PrimitiveTypeKind::PTK_UInt16) {
        auto f=SimpleArrayField::Create(name, description, count,
                                    (Smp::UInt16*)address,
                                    ptype, viewKind, type,
                                    isState, isInput, isOutput,
                                    parent);
        return f;
    }
    else if (ptype == Smp::PrimitiveTypeKind::PTK_UInt32) {
        auto f=SimpleArrayField::Create(name, description, count,
                                    (Smp::UInt32*)address,
                                    ptype, viewKind, type,
                                    isState, isInput, isOutput,
                                    parent);
        return f;
    }
    else if (ptype == Smp::PrimitiveTypeKind::PTK_UInt64) {
        auto f=SimpleArrayField::Create(name, description, count,
                                    (Smp::UInt64*)address,
                                    ptype, viewKind, type,
                                    isState, isInput, isOutput,
                                    parent);
        return f;
    }
    else if (ptype == Smp::PrimitiveTypeKind::PTK_Float32) {
        auto f=SimpleArrayField::Create(name, description, count,
                                    (Smp::Float32*)address,
                                    ptype, viewKind, type,
                                    isState, isInput, isOutput,
                                    parent);
        return f;
    }
    else if (ptype == Smp::PrimitiveTypeKind::PTK_Float64) {
        auto f=SimpleArrayField::Create(name, description, count,
                                    (Smp::Float64*)address,
                                    ptype, viewKind, type,
                                    isState, isInput, isOutput,
                                    parent);
        return f;
    }
    return nullptr;
}

// --------------------------------------------------------------------
// toprint operator
// ..........................................................
// toprint Smp::IArrayField
std::ostream& toprint(std::ostream& os, const Smp::IArrayField& obj) {
    os << "[ ";
    for (int i = 0; i < obj.GetSize(); i++) {
        if (dynamic_cast<Smp::IArrayField*>(obj.GetItem(i)) != nullptr) {
            toprint(os, *(dynamic_cast<Smp::IArrayField*>(obj.GetItem(i))));
        }
        else if (dynamic_cast<Smp::ISimpleArrayField*>(obj.GetItem(i)) != nullptr) {
            toprint(os, *(dynamic_cast<Smp::ISimpleArrayField*>(obj.GetItem(i))));
        }
        else {
            auto field = dynamic_cast<Smp::ISimpleField*>(obj.GetItem(i));
            toprint(os, *field);
        }

        if (i < obj.GetSize() - 1) {
            os << " ; ";
        }
        else {
            os << " ]";
        }
    }
    return os;
}

// toprint Smp::ISimpleArrayField
std::ostream& toprint(std::ostream& os, const Smp::ISimpleArrayField& obj) {
    os << "[";
    for (int i = 0; i < obj.GetSize() - 1; i++) {
        os << " " << obj.GetValue(i) << " ;";
    }
    os << " " << obj.GetValue(obj.GetSize() - 1) << " ]";
    return os;
}

// toprint Smp::ISimpleField
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



}  // namespace smpdk
}  // namespace simph

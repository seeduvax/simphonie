/*
 * @file Field.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/Field.hpp"
#include "simph/kern/ExInvalidTarget.hpp"
#include "simph/kern/Type.hpp"
#include "simph/sys/Logger.hpp"
#include <cstring>

namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
Field::Field(Smp::String8 name, Smp::String8 description,
        Smp::ViewKind viewKind, 
        void* address, unsigned int dataSize,
        Smp::Bool isState,
        Smp::Bool isInput,
        Smp::Bool isOutput,
        Smp::IObject* parent):
                        Persist(name,description,parent),
                        _stateType(isState),
                        _inputType(isInput),
                        _outputType(isOutput),
                        _type(),
                        _viewKind(viewKind),
                        _data(address),
                        _dataSize(dataSize),
                        _src(nullptr),
                        _forced(false),
                        _forcedValue(),
                        _targets("cnx","",this) {
}
// ..........................................................
Field::~Field() {
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
// ..........................................................
void Field::SetValue(Smp::AnySimple value)  {
    LOGW("Can't set simple value to non simple field " << GetName());
}
// ..........................................................
Smp::AnySimple Field::GetValue() const {
    LOGW("Requesting simple value from non simple field "<<GetName());
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Int64,0);
}
// --------------------------------------------------------------------
// Smp::IDataflowField implentation
// ..........................................................
void Field::Connect(Smp::IField* target) {
    auto f=dynamic_cast<Field*>(target);
    if (f!=nullptr 
            && f->_type->GetUuid()==_type->GetUuid() 
            && f->_dataSize==_dataSize
            && IsOutput() && f->IsInput()) {
        _targets.push_back(f);
    }
    else {
        throw ExInvalidTarget(this,target);
    }
}
// ..........................................................
void Field::Push() {
    for (auto f: _targets) {
        std::memcpy(f->_data,_data,_dataSize);
    }
}
// --------------------------------------------------------------------
// Smp::IForcibleField implementation
// ..........................................................
void Field::Force(Smp::AnySimple value) {
    _forcedValue=value;
    _forced=true;
}
// ..........................................................
void Field::Unforce() {
    _forced=false;
}
// ..........................................................
Smp::Bool Field::IsForced() {
    return _forcedValue;
}
// ..........................................................
void Field::Freeze() {
    Force(GetValue());
}
// ..........................................................
template<>
Smp::AnySimple TField<Smp::Char8>::GetValue() const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Char8,*_tData);
}
// ..........................................................
template<>
Smp::AnySimple TField<Smp::Bool>::GetValue() const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Bool,*_tData);
}
// ..........................................................
template<>
Smp::AnySimple TField<Smp::Int8>::GetValue() const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Int8,*_tData);
}
// ..........................................................
template<>
Smp::AnySimple TField<Smp::UInt8>::GetValue() const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_UInt8,*_tData);
}
// ..........................................................
template<>
Smp::AnySimple TField<Smp::Int16>::GetValue() const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Int16,*_tData);
}
// ..........................................................
template<>
Smp::AnySimple TField<Smp::UInt16>::GetValue() const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_UInt16,*_tData);
}
// ..........................................................
template<>
Smp::AnySimple TField<Smp::Int32>::GetValue() const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Int32,*_tData);
}
// ..........................................................
template<>
Smp::AnySimple TField<Smp::UInt32>::GetValue() const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_UInt32,*_tData);
}
// ..........................................................
template<>
Smp::AnySimple TField<Smp::Int64>::GetValue() const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Int64,*_tData);
}
// ..........................................................
template<>
Smp::AnySimple TField<Smp::UInt64>::GetValue() const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_UInt64,*_tData);
}
// ..........................................................
template<>
Smp::AnySimple TField<Smp::Float32>::GetValue() const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Float32,*_tData);
}
// ..........................................................
template<>
Smp::AnySimple TField<Smp::Float64>::GetValue() const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Float64,*_tData);
}

// ..........................................................
static Type _char8Type(Smp::Uuids::Uuid_Char8,Smp::PrimitiveTypeKind::PTK_Char8,
        sizeof(Smp::Char8),
        "Char8","One byte char data type");
static Type _boolType(Smp::Uuids::Uuid_Bool,Smp::PrimitiveTypeKind::PTK_Bool,
        sizeof(Smp::Bool),
        "Bool","Bool data type");
static Type _int8Type(Smp::Uuids::Uuid_Int8,Smp::PrimitiveTypeKind::PTK_Int8,
        sizeof(Smp::Int8),
        "Int8","One byte signed int data type");
static Type _int16Type(Smp::Uuids::Uuid_Int16,Smp::PrimitiveTypeKind::PTK_Int16,
        sizeof(Smp::Int16),
        "Int16","Two bytes signed int data type");
static Type _int32Type(Smp::Uuids::Uuid_Int32,Smp::PrimitiveTypeKind::PTK_Int32,
        sizeof(Smp::Int32),
        "Int32","Four bytes signed int data type");
static Type _int64Type(Smp::Uuids::Uuid_Int64,Smp::PrimitiveTypeKind::PTK_Int64,
        sizeof(Smp::Int64),
        "Int64","Eight bytes signed int data type");
static Type _uint8Type(Smp::Uuids::Uuid_UInt8,Smp::PrimitiveTypeKind::PTK_UInt8,
        sizeof(Smp::UInt8),
        "UInt8","One byte unsigned int data type");
static Type _uint16Type(Smp::Uuids::Uuid_UInt16,Smp::PrimitiveTypeKind::PTK_UInt16,
        sizeof(Smp::UInt16),
        "UInt16","Two bytes unsigned int data type");
static Type _uint32Type(Smp::Uuids::Uuid_UInt32,Smp::PrimitiveTypeKind::PTK_UInt32,
        sizeof(Smp::UInt32),
        "UInt32","Four bytes unsigned int data type");
static Type _uint64Type(Smp::Uuids::Uuid_UInt64,Smp::PrimitiveTypeKind::PTK_UInt64,
        sizeof(Smp::UInt64),
        "UInt64","Eight bytes unsigned int data type");
static Type _float32Type(Smp::Uuids::Uuid_Float32,Smp::PrimitiveTypeKind::PTK_Float32,
        sizeof(Smp::Float32),
        "Float32","Four bytes float data type");
static Type _float64Type(Smp::Uuids::Uuid_Float64,Smp::PrimitiveTypeKind::PTK_Float64,
        sizeof(Smp::Float64),
        "Float64","Eight bytes float data type");
// ..........................................................
template<>
void TField<Smp::Char8>::initType() {
    setType(&_char8Type);
}
template<>
void TField<Smp::Bool>::initType() {
    setType(&_boolType);
}
template<>
void TField<Smp::Int8>::initType() {
    setType(&_int8Type);
}
template<>
void TField<Smp::Int16>::initType() {
    setType(&_int16Type);
}
template<>
void TField<Smp::Int32>::initType() {
    setType(&_int32Type);
}
template<>
void TField<Smp::Int64>::initType() {
    setType(&_int64Type);
}
template<>
void TField<Smp::UInt8>::initType() {
    setType(&_uint8Type);
}
template<>
void TField<Smp::UInt16>::initType() {
    setType(&_uint16Type);
}
template<>
void TField<Smp::UInt32>::initType() {
    setType(&_uint32Type);
}
template<>
void TField<Smp::UInt64>::initType() {
    setType(&_uint64Type);
}
template<>
void TField<Smp::Float32>::initType() {
    setType(&_float32Type);
}
template<>
void TField<Smp::Float64>::initType() {
    setType(&_float64Type);
}
}} // namespace simph::kern

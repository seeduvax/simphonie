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
#include "simph/kern/StructureType.hpp"
#include "simph/sys/Logger.hpp"

namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
Field::Field(Smp::String8 name, Smp::String8 description,
        Smp::ViewKind viewKind, 
        void* address, unsigned int dataSize,
        Smp::Publication::IType* type,
        Smp::Bool isState,
        Smp::Bool isInput,
        Smp::Bool isOutput,
        Smp::IObject* parent):
                        Persist(name,description,parent),
                        _stateType(isState),
                        _inputType(isInput),
                        _outputType(isOutput),
                        _type(type),
                        _viewKind(viewKind),
                        _data(address==nullptr?malloc(dataSize):address),
                        _dataSize(dataSize),
                        _allocated(address==nullptr),
                        _src(nullptr),
                        _forced(false),
                        _forcedValue(),
                        _targets("cnx","",this) {
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
// ..........................................................
void Field::SetValue(Smp::AnySimple value)  {
    LOGW("Can't set simple value to non simple field " << GetName());
}
// ..........................................................
Smp::PrimitiveTypeKind Field::GetPrimitiveTypeKind() const {
    return Smp::PrimitiveTypeKind::PTK_None;
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
            && ( f->_type->GetUuid()==_type->GetUuid() )
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
Smp::PrimitiveTypeKind TField<Smp::Char8>::GetPrimitiveTypeKind() const {
    return Smp::PrimitiveTypeKind::PTK_Char8;
}
// ..........................................................
template<>
Smp::PrimitiveTypeKind TField<Smp::Bool>::GetPrimitiveTypeKind() const {
    return Smp::PrimitiveTypeKind::PTK_Bool;
}
// ..........................................................
template<>
Smp::PrimitiveTypeKind TField<Smp::Int8>::GetPrimitiveTypeKind() const {
    return Smp::PrimitiveTypeKind::PTK_Int8;
}
// ..........................................................
template<>
Smp::PrimitiveTypeKind TField<Smp::Int16>::GetPrimitiveTypeKind() const {
    return Smp::PrimitiveTypeKind::PTK_Int16;
}
// ..........................................................
template<>
Smp::PrimitiveTypeKind TField<Smp::Int32>::GetPrimitiveTypeKind() const {
    return Smp::PrimitiveTypeKind::PTK_Int32;
}
// ..........................................................
template<>
Smp::PrimitiveTypeKind TField<Smp::Int64>::GetPrimitiveTypeKind() const {
    return Smp::PrimitiveTypeKind::PTK_Int64;
}
// ..........................................................
template<>
Smp::PrimitiveTypeKind TField<Smp::UInt8>::GetPrimitiveTypeKind() const {
    return Smp::PrimitiveTypeKind::PTK_UInt8;
}
// ..........................................................
template<>
Smp::PrimitiveTypeKind TField<Smp::UInt16>::GetPrimitiveTypeKind() const {
    return Smp::PrimitiveTypeKind::PTK_UInt16;
}
// ..........................................................
template<>
Smp::PrimitiveTypeKind TField<Smp::UInt32>::GetPrimitiveTypeKind() const {
    return Smp::PrimitiveTypeKind::PTK_UInt32;
}
// ..........................................................
template<>
Smp::PrimitiveTypeKind TField<Smp::UInt64>::GetPrimitiveTypeKind() const {
    return Smp::PrimitiveTypeKind::PTK_UInt64;
}
// ..........................................................
template<>
Smp::PrimitiveTypeKind TField<Smp::Float32>::GetPrimitiveTypeKind() const {
    return Smp::PrimitiveTypeKind::PTK_Float32;
}
// ..........................................................
template<>
Smp::PrimitiveTypeKind TField<Smp::Float64>::GetPrimitiveTypeKind() const {
    return Smp::PrimitiveTypeKind::PTK_Float64;
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

// --------------------------------------------------------------------
// StructureField
// ..........................................................
StructureField::StructureField(Smp::String8 name,
            Smp::String8 description, Smp::ViewKind viewKind, void* address,
            Smp::Publication::IType* type,
            Smp::Bool isState, Smp::Bool isInput, Smp::Bool isOutput,
            Smp::IObject* parent): Field(name,description,viewKind,address,0,
                    type,isState,isInput,isOutput,parent) {
    auto st=dynamic_cast<StructureType*>(type);
    if (st!=nullptr) {
        st->setup(this);
    }
    else {
        LOGE("Can't setup field "<<name<<", its type "<<type->GetName()
            <<" is not an expected Structure Type.");
    }
}
// ..........................................................
StructureField::~StructureField() {
}
// ..........................................................
void StructureField::Push() {
    for (auto f: _fields) {
        f->Push();
    }
}
// ..........................................................
void StructureField::Connect(Smp::IField* target) {
    auto f=dynamic_cast<StructureField*>(target);
    if (f!=nullptr 
            && f->GetType()->GetUuid()==GetType()->GetUuid() 
            && IsOutput() && f->IsInput()
            && _fields.size()==f->_fields.size()) {
        for(int i=0;i<_fields.size();i++) {
            _fields[i]->Connect(f->_fields[i]);
        }
    }
    else {
        throw ExInvalidTarget(this,target);
    }
}
// ..........................................................
void StructureField::addField(Field* f) {
    _fields.push_back(f);
}
// --------------------------------------------------------------------
// Array Field
// ..........................................................
template<>
Smp::AnySimple SimpleArrayField<Smp::Char8>::GetValue(Smp::UInt64 index) const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Char8,_tData[index]);
}
// ..........................................................
template<>
Smp::AnySimple SimpleArrayField<Smp::Bool>::GetValue(Smp::UInt64 index) const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Bool,_tData[index]);
}
// ..........................................................
template<>
Smp::AnySimple SimpleArrayField<Smp::Int8>::GetValue(Smp::UInt64 index) const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Int8,_tData[index]);
}
// ..........................................................
template<>
Smp::AnySimple SimpleArrayField<Smp::Int16>::GetValue(Smp::UInt64 index) const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Int16,_tData[index]);
}
// ..........................................................
template<>
Smp::AnySimple SimpleArrayField<Smp::Int32>::GetValue(Smp::UInt64 index) const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Int32,_tData[index]);
}
// ..........................................................
template<>
Smp::AnySimple SimpleArrayField<Smp::Int64>::GetValue(Smp::UInt64 index) const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Int64,_tData[index]);
}
// ..........................................................
template<>
Smp::AnySimple SimpleArrayField<Smp::UInt8>::GetValue(Smp::UInt64 index) const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_UInt8,_tData[index]);
}
// ..........................................................
template<>
Smp::AnySimple SimpleArrayField<Smp::UInt16>::GetValue(Smp::UInt64 index) const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_UInt16,_tData[index]);
}
// ..........................................................
template<>
Smp::AnySimple SimpleArrayField<Smp::UInt32>::GetValue(Smp::UInt64 index) const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_UInt32,_tData[index]);
}
// ..........................................................
template<>
Smp::AnySimple SimpleArrayField<Smp::UInt64>::GetValue(Smp::UInt64 index) const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_UInt64,_tData[index]);
}
// ..........................................................
template<>
Smp::AnySimple SimpleArrayField<Smp::Float32>::GetValue(Smp::UInt64 index) const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Float32,_tData[index]);
}
// ..........................................................
template<>
Smp::AnySimple SimpleArrayField<Smp::Float64>::GetValue(Smp::UInt64 index) const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Float64,_tData[index]);
}

}} // namespace simph::kern

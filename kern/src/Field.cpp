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

// --------------------------------------------------------------------
// StructureField
// ..........................................................
StructureField::StructureField(Smp::String8 name,
            Smp::String8 description, Smp::ViewKind viewKind, void* address,
            Smp::Bool isState, Smp::Bool isInput, Smp::Bool isOutput,
            Smp::IObject* parent): Field(name,description,viewKind,address,0,
                    isState,isInput,isOutput,parent),
                    _baseAddress(address) {
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
            && f->getType()->GetUuid()==getType()->GetUuid() 
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



}} // namespace simph::kern

/*
 * @file SimpleArrayField.cpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/smpdk/SimpleArrayField.hpp"
#include "Smp/IOutputField.h"
#include "simph/smpdk/Collection.hpp"
#include "simph/smpdk/ExInvalidArrayIndex.hpp"
#include "simph/smpdk/SimpleField.hpp"

#include <sstream>

namespace simph {
namespace smpdk {

/**
 * Simple array field template
 */  
template <typename T>
class TSimpleArrayField: public SimpleArrayField {
public:
    TSimpleArrayField(Smp::String8 name, Smp::String8 description, Smp::UInt64 count, void* address,
                     Smp::PrimitiveTypeKind ptype, Smp::ViewKind viewKind, Smp::Publication::IType* type,
                     Smp::Bool isState, Smp::Bool isInput, Smp::Bool isOutput, Smp::IObject* parent)
        : SimpleArrayField(name, description, count, address,
                          sizeof(T), viewKind, type,
                          isState, isInput, isOutput,
                          parent),
          _tData((T*)address),
          _count(count) {
        _itemFields = new Smp::ISimpleField*[_count];
        for (int i = 0; i < _count; i++) {
            std::ostringstream s;
            s << "[" << i << "]";
            _itemFields[i] = SimpleField::Create(
                                            s.str().c_str(),
                                            "",
                                            viewKind,
                                            &(_tData[i]),
                                            isState,
                                            isInput,
                                            isOutput,
                                            this);
        }
    }
    virtual ~TSimpleArrayField() {
        for (int i = 0; i < _count; i++) {
            delete _itemFields[i];
        }
        delete[] _itemFields;
    }
    // Smp::ISimpleArrayField implementation
    Smp::UInt64 GetSize() const override {
        return _count;
    }
    Smp::IObject* GetChild(Smp::String8 fullName) const override {
        std::string sname=fullName;
        for (int i=0;i<_count;i++) {
            if (sname==_itemFields[i]->GetName()) {
                return _itemFields[i];
            }
        }
        return nullptr;
    }
    // Smp::ISimpleArrayField implementation
    Smp::AnySimple GetValue(Smp::UInt64 index) const override {
        return _tData[index];
    }
    void SetValue(Smp::UInt64 index, Smp::AnySimple value) override {
        _tData[index] = value;
    }
    void GetValues( Smp::UInt64 length,
                    Smp::AnySimple* values,
                    Smp::UInt64 startIndex) const override {
        int j=0;
        for (int i=startIndex; i<_count && j<length; i++) {
            values[j]=GetValue(i);
            j++;
        }
    }
    void SetValues( Smp::UInt64 length,
                    Smp::AnySimpleArray values,
                    Smp::UInt64 startIndex = 0) override {
        int j=0;
        for (int i=startIndex; i<_count && j<length; i++) {
             _tData[i] = values[j];
            j++;
        }
    }

private:
    T* _tData;
    Smp::UInt64 _count;
    Smp::ISimpleField** _itemFields;
};

/**
 * Simple Array output field template
 */
template <typename T>
class TSimpleArrayOutputField: public TSimpleArrayField<T>,
                            virtual public Smp::IOutputField {
public:
    TSimpleArrayOutputField(Smp::String8 name,
                            Smp::String8 description,
                            Smp::UInt64 count,
                            T* address,
                            Smp::PrimitiveTypeKind ptype,
                            Smp::ViewKind viewKind,
                            Smp::Publication::IType* type,
                            Smp::Bool isState,
                            Smp::Bool isInput,
                            Smp::IObject* parent)
        : TSimpleArrayField<T>(name, description, count, address, ptype,
                        viewKind, type, isState, isInput, true, parent),
          _targets("targets","Connected fields",this) {
    }
    virtual ~TSimpleArrayOutputField() {
    }
    void Connect(Smp::IField* target) override {
        auto sf=dynamic_cast<Smp::ISimpleArrayField*>(target);
        if (sf!=nullptr 
                && !_targets.contain(sf) 
                && sf->GetSize()>=this->GetSize()) {
            _targets.push_back(sf);
        }
    }
    void Disconnect(Smp::IField* target) override {
        auto sf=dynamic_cast<Smp::ISimpleArrayField*>(target);
        if (sf!=nullptr && _targets.contain(sf)) {
            _targets.remove(sf);
        }
    }
    void Push() override {
        for (auto target: _targets) {
            for (Smp::UInt64 i=0;i<this->GetSize();i++) {
                target->SetValue(i,this->GetValue(i));
            }
        }
    }
    const Smp::FieldCollection* GetInputFields() const override {
        return dynamic_cast<const Smp::FieldCollection*>(&_targets);
    }
    Smp::Bool IsAutomatic() const override {
        return false;
    }
private:
    OwnedCollection<Smp::ISimpleArrayField> _targets;
};
// --------------------------------------------------------------------
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


// toprint Smp::ISimpleArrayField
std::ostream& toprint(std::ostream& os, const Smp::ISimpleArrayField& obj) {
    os << "[";
    for (int i = 0; i < obj.GetSize() - 1; i++) {
        os << " " << obj.GetValue(i) << " ;";
    }
    os << " " << obj.GetValue(obj.GetSize() - 1) << " ]";
    return os;
}
}} // namespace simph::smpdk

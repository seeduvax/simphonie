/*
 * @file SimpleArrayField.cpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simdeck/SimpleArrayField.hpp"
#include "Smp/IOutputField.h"
#include "simdeck/Collection.hpp"
#include "simdeck/ExInvalidArrayIndex.hpp"
#include "simdeck/SimpleField.hpp"
#include "Smp/IStorageReader.h"
#include "Smp/IStorageWriter.h"

#include <sstream>

namespace simdeck {

/**
 * Simple array field template
 */  
template <typename T>
class TSimpleArrayField: public SimpleArrayField {
public:
    TSimpleArrayField(Smp::String8 name, Smp::String8 description, Smp::UInt64 count, void* address,
                     const Smp::Publication::IType* ptype, Smp::ViewKind viewKind, const Smp::Publication::IType* type,
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
                                            ptype,
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
    void Restore(Smp::IStorageReader* reader) override {
        reader->Restore(_tData, sizeof(T)*_count);
    }
    void Store(Smp::IStorageWriter* writer) override {
        writer->Store(_tData, sizeof(T)*_count);
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
                            const Smp::Publication::IType* ptype,
                            Smp::ViewKind viewKind,
                            const Smp::Publication::IType* type,
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
                dynamic_cast<Smp::ISimpleArrayField*>(target)->SetValue(i,this->GetValue(i));
            }
        }
    }
    const Smp::FieldCollection* GetInputFields() const override {
        return &_targets;
    }
    Smp::Bool IsAutomatic() const override {
        return false;
    }
private:
    Collection<Smp::IField> _targets;
};
// --------------------------------------------------------------------
// ..........................................................
SimpleArrayField::SimpleArrayField(
                      Smp::String8 name, Smp::String8 description,
                      Smp::UInt64 count, void* address, Smp::UInt64 itemSize,
                      Smp::ViewKind viewKind,
                      const Smp::Publication::IType* type,
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
                            const Smp::Publication::IType* ptype,
                            Smp::ViewKind viewKind,
                            const Smp::Publication::IType* type,
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
                            const Smp::Publication::IType* ptype,
                            Smp::ViewKind viewKind,
                            const Smp::Publication::IType* type,
                            Smp::Bool isState,
                            Smp::Bool isInput,
                            Smp::Bool isOutput,
                            Smp::IObject* parent) {
    Smp::ISimpleArrayField* f=nullptr;
    switch (ptype->GetPrimitiveTypeKind()) {
        case Smp::PrimitiveTypeKind::PTK_Bool:
             f=SimpleArrayField::Create(name, description, count, 
                                        (Smp::Bool*)address,
                                        ptype, viewKind, type,
                                        isState, isInput, isOutput,
                                        parent);
            break;
        case Smp::PrimitiveTypeKind::PTK_Char8:
            f=SimpleArrayField::Create(name, description, count,
                                        (Smp::Char8*)address,
                                        ptype, viewKind, type,
                                        isState, isInput, isOutput,
                                        parent);
            break;
        case Smp::PrimitiveTypeKind::PTK_Int8:
            f=SimpleArrayField::Create(name, description, count,
                                        (Smp::Int8*)address,
                                        ptype, viewKind, type,
                                        isState, isInput, isOutput,
                                        parent);
            break;
        case Smp::PrimitiveTypeKind::PTK_Int16:
            f=SimpleArrayField::Create(name, description, count,
                                        (Smp::Int16*)address,
                                        ptype, viewKind, type,
                                        isState, isInput, isOutput,
                                        parent);
            break;
        case Smp::PrimitiveTypeKind::PTK_Int32:
            f=SimpleArrayField::Create(name, description, count,
                                        (Smp::Int32*)address,
                                        ptype, viewKind, type,
                                        isState, isInput, isOutput,
                                        parent);
            break;
        case Smp::PrimitiveTypeKind::PTK_Int64:
            f=SimpleArrayField::Create(name, description, count,
                                        (Smp::Int64*)address,
                                        ptype, viewKind, type,
                                        isState, isInput, isOutput,
                                        parent);
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt8:
            f=SimpleArrayField::Create(name, description, count,
                                        (Smp::UInt8*)address,
                                        ptype, viewKind, type,
                                        isState, isInput, isOutput,
                                        parent);
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt16:
            f=SimpleArrayField::Create(name, description, count,
                                        (Smp::UInt16*)address,
                                        ptype, viewKind, type,
                                        isState, isInput, isOutput,
                                        parent);
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt32:
            f=SimpleArrayField::Create(name, description, count,
                                        (Smp::UInt32*)address,
                                        ptype, viewKind, type,
                                        isState, isInput, isOutput,
                                        parent);
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt64:
            f=SimpleArrayField::Create(name, description, count,
                                        (Smp::UInt64*)address,
                                        ptype, viewKind, type,
                                        isState, isInput, isOutput,
                                        parent);
            break;
        case Smp::PrimitiveTypeKind::PTK_Float32:
            f=SimpleArrayField::Create(name, description, count,
                                        (Smp::Float32*)address,
                                        ptype, viewKind, type,
                                        isState, isInput, isOutput,
                                        parent);
            break;
        case Smp::PrimitiveTypeKind::PTK_Float64:
            f=SimpleArrayField::Create(name, description, count,
                                        (Smp::Float64*)address,
                                        ptype, viewKind, type,
                                        isState, isInput, isOutput,
                                        parent);
            break;
    }
    return f;
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
} // namespace simdeck

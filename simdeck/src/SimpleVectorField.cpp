/*
 * @file SimpleVectorField.cpp
 *
 * Copyright 2026 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */

#include <sstream>
#include "simdeck/SimpleVectorField.hpp"
#include "Smp/IStorageReader.h"
#include "Smp/IStorageWriter.h"
#include "Smp/IOutputField.h"
#include "simdeck/Collection.hpp"

namespace simdeck {

/**
 *
 */
template <typename T>
// TODO constraint if possible T to be one of the primitive type kind.
class TSimpleVectorField: public SimpleVectorField, public virtual Smp::ISimpleArrayField {
public:
    /**
     * Default constructor.
     */
    TSimpleVectorField(Smp::String8 name, Smp::String8 description, std::vector<T>* address,
                     const Smp::Publication::IType* ptype, Smp::ViewKind viewKind, const Smp::Publication::IType* type,
                     Smp::Bool isState, Smp::Bool isInput, Smp::Bool isOutput, Smp::IObject* parent)
        : SimpleVectorField(name, description, address,
                          viewKind, type,
                          isState, isInput, isOutput,
                          parent),
          _tData((std::vector<T>*)address) {
        updateFields();     
    }
    /**
     * Destructor.
     */
    virtual ~TSimpleVectorField() {
    }

    // Smp::ISimpleArrayField implementation
    Smp::UInt64 GetSize() const override {
        updateFields();
        return _tData->size();
    }
    Smp::IObject* GetChild(Smp::String8 fullName) const override {
        updateFields();
        for (auto& f: _fields) {
            if (fullName==f.GetName()) {
                return &f;
            }
        }
        return nullptr;
    }
    Smp::AnySimple GetValue(Smp::UInt64 index) const override {
        return (*_tData)[index];
    }
    void SetValue(Smp::UInt64 index, Smp::AnySimple value) override {
        (*_tData)[index]=value;
    }
    void GetValues( Smp::UInt64 length,
                    Smp::AnySimple* values,
                    Smp::UInt64 startIndex = 0) const override {
        int j=0;
        for (int i=startIndex; i<_tData->size() && j<length; i++) {
            values[j]=GetValue(i);
            j++;
        } 
    }
    void SetValues( Smp::UInt64 length,
                    Smp::AnySimpleArray values,
                    Smp::UInt64 startIndex = 0) override {
        int j=0;
        for (int i=startIndex; i < _tData->size(); i++) {
            (*_tData)[i]=values[j];
            j++;
        }
    }
    void Restore(Smp::IStorageReader* reader) override {
        Smp::UInt64 size = _tData->size();
        reader->Restore(&size, sizeof(size));
        _tData->clear();
        for (int i=0; i<size; i++) {
            T item;
            reader->Restore(&item, sizeof(T));
            _tData->push_back(item);
        }
    }
    void Store(Smp::IStorageWriter* writer) override {
        Smp::UInt64 size=_tData->size();
        writer->Store(&size, sizeof(size));
        for (T item: *_tData) {
            writer->Store(&item, sizeof(T));
        }
    }

private:

    class SimpleItemField: public Field, public virtual Smp::ISimpleField {
    public:
        SimpleItemField(Smp::UInt64 index, TSimpleVectorField<T>* parent,
                Smp::ViewKind viewKind, void* address,
                const Smp::Publication::IType* type, Smp::Bool isState,
                Smp::Bool isInput, Smp::Bool isOutput):
            Field(buildName(index).c_str(),"", viewKind, address, sizeof(T),
                  type, isState, isInput, isOutput, parent),
            _parent(parent),
            _index(index) {
        }
        Smp::AnySimple GetValue() const override {
            return _parent->GetValue(_index);
        }
        void SetValue(Smp::AnySimple value) override {
            _parent->SetValue(_index, value);
        }
        Smp::PrimitiveTypeKind GetPrimitiveTypeKind() const override;

    protected:
        static std::string buildName(Smp::UInt64 index) {
            std::ostringstream oss;
            oss << '[' << index << ']';
            return oss.str();
        }
        
    private:
        TSimpleVectorField<T>* _parent;
        Smp::UInt64 _index;
    };

    void updateFields() const {
        
        if (_fields.size() != _tData->size()) {
            _fields.clear();
            for (int i=0;i<_tData->size();i++) {
                _fields.push_back(SimpleItemField(i,(TSimpleVectorField<T>*)this,
                        this->GetView(), _tData, 
                        nullptr, // should be item type
                        IsState(), IsInput(), IsOutput()));
            }
        }
    }
    std::vector<T>* _tData;
    mutable std::vector<SimpleItemField> _fields;
};

// --------------------------------------------------------------------
// ..........................................................
template<>
Smp::PrimitiveTypeKind TSimpleVectorField<Smp::Char8>::SimpleItemField::GetPrimitiveTypeKind() const {
    return Smp::PrimitiveTypeKind::PTK_Char8;
}
// ..........................................................
template<>
Smp::PrimitiveTypeKind TSimpleVectorField<Smp::Bool>::SimpleItemField::GetPrimitiveTypeKind() const {
    return Smp::PrimitiveTypeKind::PTK_Bool;
}
// ..........................................................
template<>
Smp::PrimitiveTypeKind TSimpleVectorField<Smp::Int8>::SimpleItemField::GetPrimitiveTypeKind() const {
    return Smp::PrimitiveTypeKind::PTK_Int8;
}
// ..........................................................
template<>
Smp::PrimitiveTypeKind TSimpleVectorField<Smp::Int16>::SimpleItemField::GetPrimitiveTypeKind() const {
    return Smp::PrimitiveTypeKind::PTK_Int16;
}
// ..........................................................
template<>
Smp::PrimitiveTypeKind TSimpleVectorField<Smp::Int32>::SimpleItemField::GetPrimitiveTypeKind() const {
    return Smp::PrimitiveTypeKind::PTK_Int32;
}
// ..........................................................
template<>
Smp::PrimitiveTypeKind TSimpleVectorField<Smp::Int64>::SimpleItemField::GetPrimitiveTypeKind() const {
    return Smp::PrimitiveTypeKind::PTK_Int64;
}
// ..........................................................
template<>
Smp::PrimitiveTypeKind TSimpleVectorField<Smp::UInt8>::SimpleItemField::GetPrimitiveTypeKind() const {
    return Smp::PrimitiveTypeKind::PTK_UInt8;
}
// ..........................................................
template<>
Smp::PrimitiveTypeKind TSimpleVectorField<Smp::UInt16>::SimpleItemField::GetPrimitiveTypeKind() const {
    return Smp::PrimitiveTypeKind::PTK_UInt16;
}
// ..........................................................
template<>
Smp::PrimitiveTypeKind TSimpleVectorField<Smp::UInt32>::SimpleItemField::GetPrimitiveTypeKind() const {
    return Smp::PrimitiveTypeKind::PTK_UInt32;
}
// ..........................................................
template<>
Smp::PrimitiveTypeKind TSimpleVectorField<Smp::UInt64>::SimpleItemField::GetPrimitiveTypeKind() const {
    return Smp::PrimitiveTypeKind::PTK_UInt64;
}
// ..........................................................
template<>
Smp::PrimitiveTypeKind TSimpleVectorField<Smp::Float32>::SimpleItemField::GetPrimitiveTypeKind() const {
    return Smp::PrimitiveTypeKind::PTK_Float32;
}
// ..........................................................
template<>
Smp::PrimitiveTypeKind TSimpleVectorField<Smp::Float64>::SimpleItemField::GetPrimitiveTypeKind() const {
    return Smp::PrimitiveTypeKind::PTK_Float64;
}
// ..........................................................
template <>
Smp::AnySimple TSimpleVectorField<Smp::Char8>::GetValue(Smp::UInt64 index) const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Char8, (*_tData)[index]);
}
// ..........................................................
template <>
Smp::AnySimple TSimpleVectorField<Smp::Bool>::GetValue(Smp::UInt64 index) const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Bool, (*_tData)[index]);
}
// ..........................................................
template <>
Smp::AnySimple TSimpleVectorField<Smp::Int8>::GetValue(Smp::UInt64 index) const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Int8, (*_tData)[index]);
}
// ..........................................................
template <>
Smp::AnySimple TSimpleVectorField<Smp::Int16>::GetValue(Smp::UInt64 index) const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Int16, (*_tData)[index]);
}
// ..........................................................
template <>
Smp::AnySimple TSimpleVectorField<Smp::Int32>::GetValue(Smp::UInt64 index) const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Int32, (*_tData)[index]);
}
// ..........................................................
template <>
Smp::AnySimple TSimpleVectorField<Smp::Int64>::GetValue(Smp::UInt64 index) const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Int64, (*_tData)[index]);
}
// ..........................................................
template <>
Smp::AnySimple TSimpleVectorField<Smp::UInt8>::GetValue(Smp::UInt64 index) const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_UInt8, (*_tData)[index]);
}
// ..........................................................
template <>
Smp::AnySimple TSimpleVectorField<Smp::UInt16>::GetValue(Smp::UInt64 index) const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_UInt16, (*_tData)[index]);
}
// ..........................................................
template <>
Smp::AnySimple TSimpleVectorField<Smp::UInt32>::GetValue(Smp::UInt64 index) const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_UInt32, (*_tData)[index]);
}
// ..........................................................
template <>
Smp::AnySimple TSimpleVectorField<Smp::UInt64>::GetValue(Smp::UInt64 index) const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_UInt64, (*_tData)[index]);
}
// ..........................................................
template <>
Smp::AnySimple TSimpleVectorField<Smp::Float32>::GetValue(Smp::UInt64 index) const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Float32, (*_tData)[index]);
}
// ..........................................................
template <>
Smp::AnySimple TSimpleVectorField<Smp::Float64>::GetValue(Smp::UInt64 index) const {
    return Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Float64, (*_tData)[index]);
}
// ..........................................................
template <typename T>
// TODO constraint if possible T to be one of the primitive type kind.
class TSimpleVectorOutputField: public TSimpleVectorField<T>, public virtual Smp::IOutputField {
public:
    /**
     * constructor.
     */
    TSimpleVectorOutputField(Smp::String8 name, Smp::String8 description, std::vector<T>* address,
                     const Smp::Publication::IType* ptype, Smp::ViewKind viewKind, const Smp::Publication::IType* type,
                     Smp::Bool isState, Smp::Bool isInput, Smp::IObject* parent)
        : TSimpleVectorField<T>(name, description, address,
                          ptype, viewKind, type,
                          isState, isInput, true,
                          parent) {
    }
    /**
     * Destructor.
     */
    virtual ~TSimpleVectorOutputField() {
    }
    // Smp::IOutputField implementation
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
            auto t=dynamic_cast<Smp::ISimpleArrayField*>(target);
            auto max=this->GetSize()<t->GetSize()?this->GetSize():t->GetSize();
// TODO define a better copy implementation when target is the exact same type
            for (Smp::UInt64 i=0;i<max;i++) {
                t->SetValue(i,this->GetValue(i));
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
SimpleVectorField::SimpleVectorField(
                      Smp::String8 name, Smp::String8 description,
                      void* address,
                      Smp::ViewKind viewKind,
                      const Smp::Publication::IType* type,
                      Smp::Bool isState, Smp::Bool isInput, Smp::Bool isOutput,
                      Smp::IObject* parent)
                    : Field(name, description, viewKind, address, 
                            0, type, isState, isInput,
                            isOutput, parent) {
}
// ..........................................................
SimpleVectorField::~SimpleVectorField() {
}
// ..........................................................
template <typename T>
Smp::ISimpleArrayField* SimpleVectorField::Create(
                            Smp::String8 name,
                            Smp::String8 description,
                            std::vector<T>* address,
                            const Smp::Publication::IType* ptype,
                            Smp::ViewKind viewKind,
                            const Smp::Publication::IType* type,
                            Smp::Bool isState,
                            Smp::Bool isInput,
                            Smp::Bool isOutput,
                            Smp::IObject* parent) {
    if (isOutput) {
        return new TSimpleVectorOutputField<T>(
                        name, description,
                        static_cast<std::vector<T>*>(address), ptype, viewKind, type,
                        isState, isInput, parent);
    }
    else {
        return new TSimpleVectorField<T>(
                        name, description, 
                        static_cast<std::vector<T>*>(address), ptype, viewKind, type,
                        isState, isInput, false, parent);
    }
}
// ..........................................................
Smp::ISimpleArrayField* SimpleVectorField::Create(
                            Smp::String8 name,
                            Smp::String8 description,
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
             f=SimpleVectorField::Create(name, description, 
                                        (std::vector<Smp::Bool>*)address,
                                        ptype, viewKind, type,
                                        isState, isInput, isOutput,
                                        parent);
            break;
        case Smp::PrimitiveTypeKind::PTK_Char8:
            f=SimpleVectorField::Create(name, description,
                                        (std::vector<Smp::Char8>*)address,
                                        ptype, viewKind, type,
                                        isState, isInput, isOutput,
                                        parent);
            break;
        case Smp::PrimitiveTypeKind::PTK_Int8:
            f=SimpleVectorField::Create(name, description,
                                        (std::vector<Smp::Int8>*)address,
                                        ptype, viewKind, type,
                                        isState, isInput, isOutput,
                                        parent);
            break;
        case Smp::PrimitiveTypeKind::PTK_Int16:
            f=SimpleVectorField::Create(name, description,
                                        (std::vector<Smp::Int16>*)address,
                                        ptype, viewKind, type,
                                        isState, isInput, isOutput,
                                        parent);
            break;
        case Smp::PrimitiveTypeKind::PTK_Int32:
            f=SimpleVectorField::Create(name, description,
                                        (std::vector<Smp::Int32>*)address,
                                        ptype, viewKind, type,
                                        isState, isInput, isOutput,
                                        parent);
            break;
        case Smp::PrimitiveTypeKind::PTK_Int64:
            f=SimpleVectorField::Create(name, description,
                                        (std::vector<Smp::Int64>*)address,
                                        ptype, viewKind, type,
                                        isState, isInput, isOutput,
                                        parent);
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt8:
            f=SimpleVectorField::Create(name, description,
                                        (std::vector<Smp::UInt8>*)address,
                                        ptype, viewKind, type,
                                        isState, isInput, isOutput,
                                        parent);
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt16:
            f=SimpleVectorField::Create(name, description,
                                        (std::vector<Smp::UInt16>*)address,
                                        ptype, viewKind, type,
                                        isState, isInput, isOutput,
                                        parent);
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt32:
            f=SimpleVectorField::Create(name, description,
                                        (std::vector<Smp::UInt32>*)address,
                                        ptype, viewKind, type,
                                        isState, isInput, isOutput,
                                        parent);
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt64:
            f=SimpleVectorField::Create(name, description,
                                        (std::vector<Smp::UInt64>*)address,
                                        ptype, viewKind, type,
                                        isState, isInput, isOutput,
                                        parent);
            break;
        case Smp::PrimitiveTypeKind::PTK_Float32:
            f=SimpleVectorField::Create(name, description,
                                        (std::vector<Smp::Float32>*)address,
                                        ptype, viewKind, type,
                                        isState, isInput, isOutput,
                                        parent);
            break;
        case Smp::PrimitiveTypeKind::PTK_Float64:
            f=SimpleVectorField::Create(name, description,
                                        (std::vector<Smp::Float64>*)address,
                                        ptype, viewKind, type,
                                        isState, isInput, isOutput,
                                        parent);
            break;
    }
    return f;
}
} // namespace simdeck

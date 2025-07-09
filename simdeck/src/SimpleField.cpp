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
#include "Smp/IStorageReader.h"
#include "Smp/IStorageWriter.h"


namespace simdeck {
template <typename T>
class MemFieldCopy;

template <typename T>
class TSimpleField : public SimpleField, virtual public Smp::ISimpleField {
  friend class MemFieldCopy<T>;
public:
    TSimpleField(Smp::String8 name, Smp::String8 description, Smp::ViewKind viewKind, T* address, Smp::Bool isState,
           Smp::Bool isInput, Smp::Bool isOutput, Smp::IObject* parent, const Smp::Publication::IType* type)
        : SimpleField(name, description, viewKind, (void*)address, sizeof(T), type, isState, isInput, isOutput, parent),
          _tData(address) {
    }
    virtual ~TSimpleField() {}
    // Smp::ISimpleField implementation
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
    void Restore(Smp::IStorageReader* reader) override {
        reader->Restore(_tData, sizeof(T));
    }
    void Store(Smp::IStorageWriter* writer) override {
        writer->Store(_tData, sizeof(T));
    }

private:
    T* _tData;
    T _forcedValue{0};
    Smp::Bool _forced{false};
};
class FieldCopy {
public:
    FieldCopy(Smp::IField* from, Smp::IField* to): _from(from), _to(to) {
    }
    virtual ~FieldCopy() {
    }
    virtual void copy()=0;
    inline Smp::IField* getSource() {
        return _from;
    }
    inline Smp::IField* getTarget() {
        return _to;
    }
private:
    Smp::IField* _from;
    Smp::IField* _to;
};
template <typename T>
class MemFieldCopy: public FieldCopy {
public:
    MemFieldCopy(TSimpleField<T>* from, TSimpleField<T>* to):
                FieldCopy(from, to),  _from(from->_tData), _to(to->_tData) {
    }
    virtual ~MemFieldCopy() {
    }
    void copy() override {
        memcpy(_to, _from, sizeof(T));
    }
private:
    void* _from;
    void* _to;
};
class SimpleFieldCopy: public FieldCopy {
public:
    SimpleFieldCopy(Smp::ISimpleField* from, Smp::ISimpleField* to): 
                FieldCopy(from, to), _from(from), _to(to) {
    }
    virtual ~SimpleFieldCopy() {
    }
    void copy() override {
        _to->SetValue(_from->GetValue());
    }
private:
    Smp::ISimpleField* _from;
    Smp::ISimpleField* _to;
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
                Smp::IObject* parent,
                const Smp::Publication::IType* type
                ):  TSimpleField<T>(name, description, viewKind, address,
                         isState, false, true, parent, type),
                    _targets() {
    } 
    virtual ~TSimpleOutputField() {
        for (auto h: _copyHandlers) {
            delete h;
        }
    }

    // Smp::IOutputField implementation
    void Connect(Smp::IField* target) override {
        auto sf=dynamic_cast<Smp::ISimpleField*>(target);
        if (sf!=nullptr && !_targets.contain(sf)) {
            auto st=dynamic_cast<TSimpleField<T>*>(sf);
            if (st!=nullptr) {
                _copyHandlers.push_back(new MemFieldCopy<T>(this, st));
            }
            else {
                _copyHandlers.push_back(new SimpleFieldCopy(this, sf));
            }
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
            auto it=_copyHandlers.begin();
            for (;it!=_copyHandlers.end()&&(*it)->getTarget()!=target; ++it) {
                // just iterate until element is found.
            }
            if (it!=_copyHandlers.end()) {
                _copyHandlers.erase(it);
                delete *it;
            }
        }
    }
    void Push() override {
        for (auto h: _copyHandlers) {
            h->copy();
        }
/*
        for (auto target: _targets) {
            // TODO should not need to cast anything here, find a way to have
            // a fully resolved collection type while being able to return
            // properly the field collection whith GetInputFields 
            auto sf=dynamic_cast<Smp::ISimpleField*>(target);
            if (sf!=nullptr) {
                sf->SetValue(this->GetValue());
            }
        }
*/
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
    std::vector<FieldCopy*> _copyHandlers;
    
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
                            Smp::IObject* parent,
                            const Smp::Publication::IType* type) {
    if (isOutput) {
        return new TSimpleOutputField<T>(
                        name, description, viewKind, static_cast<T*>(address),
                        isState, isInput, parent, type);
    }
    else {
        return new TSimpleField<T>(
                        name, description, viewKind, static_cast<T*>(address),
                        isState, isInput, false, parent, type);
    }
}
// --------------------------------------------------------------------
// ..........................................................
SimpleField::SimpleField(
             Smp::String8 name, Smp::String8 description,
             Smp::ViewKind viewKind, void* address, unsigned int dataSize,
             const Smp::Publication::IType* type, Smp::Bool isState,
             Smp::Bool isInput, Smp::Bool isOutput,
             Smp::IObject* parent)
            : Field(name, description, viewKind, address, dataSize, type,
                    isState, isInput, isOutput, parent) {
}
// ..........................................................
SimpleField::~SimpleField() {
}
// ..........................................................
Smp::PrimitiveTypeKind SimpleField::GetPrimitiveTypeKind() const {
    return GetType()->GetPrimitiveTypeKind();
}
// ..........................................................
Smp::ISimpleField* SimpleField::Create(
                            Smp::String8 name,
                            Smp::String8 description,
                            Smp::ViewKind viewKind,
                            const Smp::Publication::IType* type,
                            void* address,
                            Smp::Bool isState,
                            Smp::Bool isInput,
                            Smp::Bool isOutput,
                            Smp::IObject* parent) {
    Smp::ISimpleField* f=nullptr;
    switch (type->GetPrimitiveTypeKind()) {
        case Smp::PrimitiveTypeKind::PTK_Bool:
            f=SimpleField::Create(name, description, viewKind, 
                                        (Smp::Bool*)address,
                                        isState, isInput, isOutput,
                                        parent, type);
            break;
        case Smp::PrimitiveTypeKind::PTK_Char8:
            f=SimpleField::Create(name, description, viewKind,
                                        (Smp::Char8*)address,
                                        isState, isInput, isOutput,
                                        parent, type);
            break;
        case Smp::PrimitiveTypeKind::PTK_Int8:
            f=SimpleField::Create(name, description, viewKind,
                                        (Smp::Int8*)address,
                                        isState, isInput, isOutput,
                                        parent, type);
            break;
        case Smp::PrimitiveTypeKind::PTK_Int16:
            f=SimpleField::Create(name, description, viewKind,
                                        (Smp::Int16*)address,
                                        isState, isInput, isOutput,
                                        parent, type);
            break;
        case Smp::PrimitiveTypeKind::PTK_Int32:
            f=SimpleField::Create(name, description, viewKind,
                                        (Smp::Int32*)address,
                                        isState, isInput, isOutput,
                                        parent, type);
            break;
        case Smp::PrimitiveTypeKind::PTK_Int64:
            f=SimpleField::Create(name, description, viewKind,
                                        (Smp::Int64*)address,
                                        isState, isInput, isOutput,
                                        parent, type);
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt8:
            f=SimpleField::Create(name, description, viewKind,
                                        (Smp::UInt8*)address,
                                        isState, isInput, isOutput,
                                        parent, type);
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt16:
            f=SimpleField::Create(name, description, viewKind,
                                        (Smp::UInt16*)address,
                                        isState, isInput, isOutput,
                                        parent, type);
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt32:
            f=SimpleField::Create(name, description, viewKind,
                                        (Smp::UInt32*)address,
                                        isState, isInput, isOutput,
                                        parent, type);
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt64:
            f=SimpleField::Create(name, description, viewKind,
                                        (Smp::UInt64*)address,
                                        isState, isInput, isOutput,
                                        parent, type);
            break;
        case Smp::PrimitiveTypeKind::PTK_Float32:
            f=SimpleField::Create(name, description, viewKind,
                                        (Smp::Float32*)address,
                                        isState, isInput, isOutput,
                                        parent, type);
            break;
        case Smp::PrimitiveTypeKind::PTK_Float64:
            f=SimpleField::Create(name, description, viewKind,
                                       (Smp::Float64*)address,
                                       isState, isInput, isOutput,
                                       parent, type);
            break;
    }
    // TODO set field type with the one received.
    return f;
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
} // namespace simdeck

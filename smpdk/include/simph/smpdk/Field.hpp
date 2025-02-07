/*
 * @file Field.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_smpdk_Field_HPP__
#define __simph_smpdk_Field_HPP__
#include <cstring>
#include <sstream>
#include "Smp/IForcibleField.h"
#include "Smp/IOutputField.h"
#include "Smp/ISimpleArrayField.h"
#include "Smp/IArrayField.h"
#include "Smp/ISimpleField.h"
#include "simph/smpdk/ExInvalidArrayIndex.hpp"
#include "simph/smpdk/Persist.hpp"
#include "simph/smpdk/Collection.hpp"
#include "simph/smpdk/Type.hpp"

namespace simph {
namespace smpdk {
/**
 * Concrete IField, IPersist and IForcibleField
 */
class Field : public Persist, virtual public Smp::IField {
public:
    /**
     * Destructor.
     */
    virtual ~Field();
    // Smp::IField implementation
    Smp::ViewKind GetView() const override;
    Smp::Bool IsState() const override;
    Smp::Bool IsInput() const override;
    Smp::Bool IsOutput() const override;
    const Smp::Publication::IType* GetType() const override;

protected:
    /**
     * Create a field from existing memory space or using its own memory space.
     * This constructor is protected since this Field class is not yet fully
     * concrete but is supposed to be extended.
     * @param name field name
     * @param description field destriction
     * @param viewKind field view kind.
     * @param address to wrap memory space address or nullptr to let the field
     *        allocate itself the memory.
     * @param dataSize to wrap memory space size.
     * @param type data field type
     * @param isState state flag.
     * @param isInput input flag.
     * @param isOutput output flag.
     */
    Field(Smp::String8 name, Smp::String8 description, Smp::ViewKind viewKind, void* address, unsigned int dataSize,
          Smp::Publication::IType* type, Smp::Bool isState, Smp::Bool isInput, Smp::Bool isOutput,
          Smp::IObject* parent);
    inline void setType(Smp::Publication::IType* type) {
        _type = type;
    }
    inline const void* getDataPtr() {
        return _data;
    }

private:
    /** state flag */
    Smp::Bool _stateType;
    /** input flag */
    Smp::Bool _inputType;
    /** output flag */
    Smp::Bool _outputType;
    /** field type */
    const Smp::Publication::IType* _type;
    /** view kind */
    Smp::ViewKind _viewKind;
    /** data address */
    void* _data;
    /** data byte size */
    unsigned int _dataSize;
    /** memory owner flag */
    bool _allocated;
};

class SimpleField : public Field, virtual public Smp::IForcibleField {
public:
    /**
     * Destructor.
     */
    virtual ~SimpleField();
    template <typename T>
    static IField* Create(  Smp::String8 name,
                            Smp::String8 description,
                            Smp::ViewKind viewKind,
                            T* address,
                            Smp::Bool isState,
                            Smp::Bool isInput,
                            Smp::Bool isOutput,
                            Smp::IObject* parent);
    static IField* Create(  Smp::String8 name,
                            Smp::String8 description,
                            Smp::ViewKind viewKind,
                            void* address,
                            Smp::Bool isState,
                            Smp::Bool isInput,
                            Smp::Bool isOutput,
                            Smp::IObject* parent,
                            Smp::Uuid typeUuid);
    Smp::PrimitiveTypeKind GetPrimitiveTypeKind() const override;
protected:
    SimpleField(Smp::String8 name, Smp::String8 description,
                Smp::ViewKind viewKind, void* address, unsigned int dataSize,
                Smp::Publication::IType* type, Smp::Bool isState,
                Smp::Bool isInput, Smp::Bool isOutput,
                Smp::IObject* parent);

};

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
    Smp::Bool IsForced() override {
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
    }
    void Disconnect(Smp::IField* target) override {
        auto sf=dynamic_cast<Smp::ISimpleField*>(target);
        if (sf!=nullptr && _targets.contain(sf)) {
            _targets.remove(sf);
        }
    }
    void Push() override {
        for (auto target: _targets) {
            target->SetValue(this->GetValue());
        }
    }
    const Smp::FieldCollection* GetInputFields() const {
        return dynamic_cast<const Smp::FieldCollection*>(&_targets);
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
    OwnedCollection<Smp::ISimpleField> _targets;
    
};

template <typename T>
Smp::IField* SimpleField::Create(
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

class StructureField : public Field {
public:
    StructureField(Smp::String8 name, Smp::String8 description, Smp::ViewKind viewKind, void* address,
                   Smp::Publication::IType* type, Smp::Bool isState, Smp::Bool isInput, Smp::Bool isOutput,
                   Smp::IObject* parent);
    virtual ~StructureField();
    void addField(Smp::IField* f);
    inline const void* getAddress(Smp::Int64 offset = 0) {
        return (const void*)((int64_t)getDataPtr() + offset);
    }
    inline const std::vector<Smp::IField*> getFields() const {
        return _fields;
    }

private:
    std::vector<Smp::IField*> _fields;
};

class SimpleArrayField : public Field, public virtual Smp::ISimpleArrayField, public virtual Smp::IArrayField {
public:
    template <typename T>
    static Smp::ISimpleArrayField* Create(
                      Smp::String8 name, Smp::String8 description,
                      Smp::UInt64 count, T* address,
                      Smp::PrimitiveTypeKind ptype, Smp::ViewKind viewKind,
                      Smp::Publication::IType* type,
                      Smp::Bool isState, Smp::Bool isInput, Smp::Bool isOutput,
                      Smp::IObject* parent);
    static Smp::ISimpleArrayField* Create(
                      Smp::String8 name, Smp::String8 description,
                      Smp::UInt64 count, void* address,
                      Smp::PrimitiveTypeKind ptype, Smp::ViewKind viewKind,
                      Smp::Publication::IType* type,
                      Smp::Bool isState, Smp::Bool isInput, Smp::Bool isOutput,
                      Smp::IObject* parent);
    virtual ~SimpleArrayField();
protected:
    SimpleArrayField(Smp::String8 name, Smp::String8 description,
                      Smp::UInt64 count, void* address, Smp::UInt64 itemSize,
                      Smp::ViewKind viewKind,
                      Smp::Publication::IType* type,
                      Smp::Bool isState, Smp::Bool isInput, Smp::Bool isOutput,
                      Smp::IObject* parent);
};

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
        _itemFields = new TSimpleField<T>*[_count];
        for (int i = 0; i < _count; i++) {
            std::ostringstream s;
            s << "[" << i << "]";
            _itemFields[i] =
                isOutput ? 
                new TSimpleOutputField<T>(s.str().c_str(), "", viewKind, &(_tData[i]), isState, isInput, this)
                : new TSimpleField<T>(s.str().c_str(), "", viewKind, &(_tData[i]), isState, isInput, isOutput, this);
        }
    }
    virtual ~TSimpleArrayField() {
        for (int i = 0; i < _count; i++) {
            delete _itemFields[i];
        }
        delete[] _itemFields;
    }
    // Smp::IArrayField implementation
    Smp::UInt64 GetSize() const override {
        return _count;
    }
    Smp::IField* GetItem(Smp::UInt64 index) const override {
        if (index < _count) {
            return _itemFields[index];
        }
        throw ExInvalidArrayIndex(this, index, _count);
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
    TSimpleField<T>** _itemFields;
};

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

std::ostream& toprint(std::ostream& os, const Smp::IArrayField& obj);
std::ostream& toprint(std::ostream& os, const Smp::ISimpleArrayField& obj);
std::ostream& toprint(std::ostream& os, const Smp::ISimpleField& obj);

// --------------------------------------------------------------------
// Primitive type instanciation
// ..........................................................
static Type _char8Type(Smp::Uuids::Uuid_Char8, Smp::PrimitiveTypeKind::PTK_Char8, sizeof(Smp::Char8), "Char8",
                       "One byte char data type");
static Type _boolType(Smp::Uuids::Uuid_Bool, Smp::PrimitiveTypeKind::PTK_Bool, sizeof(Smp::Bool), "Bool",
                      "Bool data type");
static Type _int8Type(Smp::Uuids::Uuid_Int8, Smp::PrimitiveTypeKind::PTK_Int8, sizeof(Smp::Int8), "Int8",
                      "One byte signed int data type");
static Type _int16Type(Smp::Uuids::Uuid_Int16, Smp::PrimitiveTypeKind::PTK_Int16, sizeof(Smp::Int16), "Int16",
                       "Two bytes signed int data type");
static Type _int32Type(Smp::Uuids::Uuid_Int32, Smp::PrimitiveTypeKind::PTK_Int32, sizeof(Smp::Int32), "Int32",
                       "Four bytes signed int data type");
static Type _int64Type(Smp::Uuids::Uuid_Int64, Smp::PrimitiveTypeKind::PTK_Int64, sizeof(Smp::Int64), "Int64",
                       "Eight bytes signed int data type");
static Type _uint8Type(Smp::Uuids::Uuid_UInt8, Smp::PrimitiveTypeKind::PTK_UInt8, sizeof(Smp::UInt8), "UInt8",
                       "One byte unsigned int data type");
static Type _uint16Type(Smp::Uuids::Uuid_UInt16, Smp::PrimitiveTypeKind::PTK_UInt16, sizeof(Smp::UInt16), "UInt16",
                        "Two bytes unsigned int data type");
static Type _uint32Type(Smp::Uuids::Uuid_UInt32, Smp::PrimitiveTypeKind::PTK_UInt32, sizeof(Smp::UInt32), "UInt32",
                        "Four bytes unsigned int data type");
static Type _uint64Type(Smp::Uuids::Uuid_UInt64, Smp::PrimitiveTypeKind::PTK_UInt64, sizeof(Smp::UInt64), "UInt64",
                        "Eight bytes unsigned int data type");
static Type _float32Type(Smp::Uuids::Uuid_Float32, Smp::PrimitiveTypeKind::PTK_Float32, sizeof(Smp::Float32), "Float32",
                         "Four bytes float data type");
static Type _float64Type(Smp::Uuids::Uuid_Float64, Smp::PrimitiveTypeKind::PTK_Float64, sizeof(Smp::Float64), "Float64",
                         "Eight bytes float data type");

}  // namespace smpdk
}  // namespace simph
#endif  // __simph_smpdk_Field_HPP__

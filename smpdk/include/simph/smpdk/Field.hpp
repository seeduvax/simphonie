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
 *
 */
class Field : public Persist, virtual public Smp::IForcibleField {
public:
    /**
     * Default constructor.
     */
    Field(Smp::String8 name, Smp::String8 description, Smp::ViewKind viewKind, void* address, unsigned int dataSize,
          Smp::Publication::IType* type, Smp::Bool isState, Smp::Bool isInput, Smp::Bool isOutput,
          Smp::IObject* parent);
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
    // Smp::IForcibleField implementation
    void Force(Smp::AnySimple value) override;
    void Unforce() override;
    Smp::Bool IsForced() override;
    void Freeze() override;
    Smp::PrimitiveTypeKind GetPrimitiveTypeKind() const override;
    Smp::AnySimple GetValue() const override;
    void SetValue(Smp::AnySimple value) override;
    template <typename T>
    static IField* Create(  Smp::String8 name,
                            Smp::String8 description,
                            Smp::ViewKind viewKind,
                            void* address,
                            Smp::Bool isState,
                            Smp::Bool isInput,
                            Smp::Bool isOutput,
                            Smp::IObject* parent);

protected:
    inline void setType(Smp::Publication::IType* type) {
        _type = type;
    }
    inline const void* getDataPtr() {
        return _data;
    }

private:
    Smp::Bool _stateType;
    Smp::Bool _inputType;
    Smp::Bool _outputType;
    const Smp::Publication::IType* _type;
    Smp::ViewKind _viewKind;
    void* _data;
    unsigned int _dataSize;
    bool _allocated;
    Field* _src;
    Smp::Bool _forced;
    Smp::AnySimple _forcedValue;
    Collection<Field> _targets;
};

template <typename T>
class TField : public Field {
public:
    TField(Smp::String8 name, Smp::String8 description, Smp::ViewKind viewKind, T* address, Smp::Bool isState,
           Smp::Bool isInput, Smp::Bool isOutput, Smp::IObject* parent)
        : Field(name, description, viewKind, (void*)address, sizeof(T), nullptr, isState, isInput, isOutput, parent),
          _tData(address) {
        initType();
    }
    virtual ~TField() {}
    // Smp::ISimpleField implementation
    Smp::PrimitiveTypeKind GetPrimitiveTypeKind() const override;
    Smp::AnySimple GetValue() const override;

    void SetValue(Smp::AnySimple value) override {
        *_tData = value;
    }
    void initType();

private:
    T* _tData;
};
template <typename T>
class TOutputField : public TField<T>, virtual public Smp::IOutputField {
public:
    TOutputField(Smp::String8 name,
                Smp::String8 description,
                Smp::ViewKind viewKind,
                T* address,
                Smp::Bool isState,
                Smp::Bool isInput,
                Smp::Bool isOutput,
                Smp::IObject* parent
                ):  TField<T>(name, description, viewKind, address,
                         isState, isInput, isOutput, parent),
                    _targets("targets","connected fields",this) {
    } 
    virtual ~TOutputField() {
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
    ::simph::smpdk::Collection<Smp::ISimpleField> _targets;
    
};

template <typename T>
Smp::IField* Field::Create(
                            Smp::String8 name,
                            Smp::String8 description,
                            Smp::ViewKind viewKind,
                            void* address,
                            Smp::Bool isState,
                            Smp::Bool isInput,
                            Smp::Bool isOutput,
                            Smp::IObject* parent) {
    if (isOutput) {
        return new TOutputField<T>(
                        name, description, viewKind, static_cast<T*>(address),
                        isState, isInput, isOutput, parent);
    }
    else {
        return new TField<T>(
                        name, description, viewKind, static_cast<T*>(address),
                        isState, isInput, isOutput, parent);
    }
}

class StructureField : public Field {
public:
    StructureField(Smp::String8 name, Smp::String8 description, Smp::ViewKind viewKind, void* address,
                   Smp::Publication::IType* type, Smp::Bool isState, Smp::Bool isInput, Smp::Bool isOutput,
                   Smp::IObject* parent);
    virtual ~StructureField();
    void addField(Field* f);
    inline const void* getAddress(Smp::Int64 offset = 0) {
        return (const void*)((int64_t)getDataPtr() + offset);
    }
    inline const std::vector<Field*> getFields() const {
        return _fields;
    }

private:
    std::vector<Field*> _fields;
};

template <typename T>
class SimpleArrayField : public Field, public virtual Smp::ISimpleArrayField, public virtual Smp::IArrayField {
public:
    SimpleArrayField(Smp::String8 name, Smp::String8 description, Smp::UInt64 count, void* address,
                     Smp::PrimitiveTypeKind ptype, Smp::ViewKind viewKind, Smp::Publication::IType* type,
                     Smp::Bool isState, Smp::Bool isInput, Smp::Bool isOutput, Smp::IObject* parent)
        : Field(name, description, viewKind, address, sizeof(T) * count, type, isState, isInput, isOutput, parent),
          _tData((T*)address),
          _count(count),
          _primitiveType(ptype) {
        _itemFields = new TField<T>*[_count];
        for (int i = 0; i < _count; i++) {
            std::ostringstream s;
            s << name << "[" << i << "]";
            _itemFields[i] =
                new TField<T>(s.str().c_str(), "", viewKind, &(_tData[i]), isState, isInput, isOutput, this);
        }
    }
    virtual ~SimpleArrayField() {
        for (int i = 0; i < _count; i++) {
            delete _itemFields[i];
        }
        delete[] _itemFields;
    }
    // specialization
    Smp::PrimitiveTypeKind GetPrimitiveTypeKind() const override {
        return _primitiveType;
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
        std::string sname=GetName();
        sname+=fullName;
// TODO TBC such element name handling is OK
        for (int i=0;i<_count;i++) {
            if (sname==_itemFields[i]->GetName()) {
                return _itemFields[i];
            }
        }
        return nullptr;
    }
    // Smp::ISimpleArrayField implementation
    Smp::AnySimple GetValue(Smp::UInt64 index) const override;
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
    Smp::PrimitiveTypeKind _primitiveType;
    TField<T>** _itemFields;
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

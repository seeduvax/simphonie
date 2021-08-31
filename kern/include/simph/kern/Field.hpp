/*
 * @file Field.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_Field_HPP__
#define __simph_kern_Field_HPP__
#include <cstring>
#include <sstream>
#include "Smp/IForcibleField.h"
#include "Smp/ISimpleArrayField.h"
#include "Smp/ISimpleField.h"
#include "simph/kern/ExInvalidArrayIndex.hpp"
#include "simph/kern/Persist.hpp"
#include "simph/smpdk/Collection.hpp"
// workaround Smp headers issue.
namespace Smp {
class IDataflowField;
}
#include "Smp/IDataflowField.h"

namespace simph {
namespace kern {
using namespace simph::smpdk;
/**
 *
 */
class Field : public Persist, virtual public Smp::IDataflowField, virtual public Smp::IForcibleField {
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
    void Connect(Smp::IField* target) override;
    void Push() override;
    // Smp::IForcibleField implementation
    void Force(Smp::AnySimple value) override;
    void Unforce() override;
    Smp::Bool IsForced() override;
    void Freeze() override;
    Smp::PrimitiveTypeKind GetPrimitiveTypeKind() const override;
    Smp::AnySimple GetValue() const override;
    void SetValue(Smp::AnySimple value) override;

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

    void SetValue(Smp::AnySimple value) {
        *_tData = value;
    }
    void initType();

private:
    T* _tData;
};

class StructureField : public Field {
public:
    StructureField(Smp::String8 name, Smp::String8 description, Smp::ViewKind viewKind, void* address,
                   Smp::Publication::IType* type, Smp::Bool isState, Smp::Bool isInput, Smp::Bool isOutput,
                   Smp::IObject* parent);
    virtual ~StructureField();
    void Push() override;
    void Connect(Smp::IField* target) override;
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
    // Smp::ISimpleArrayField implementation
    Smp::AnySimple GetValue(Smp::UInt64 index) const override;
    void SetValue(Smp::UInt64 index, Smp::AnySimple value) override {
        _tData[index] = value;
    }
    void GetValues(Smp::UInt64 length, Smp::AnySimpleArray values) const override {
        Smp::UInt64 c = length > _count ? _count : length;
        std::memcpy(values, _tData, c * sizeof(T));
    }
    void SetValues(Smp::UInt64 length, Smp::AnySimpleArray values) override {
        Smp::UInt64 c = length > _count ? _count : length;
        std::memcpy(_tData, values, c * sizeof(T));
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

}  // namespace kern
}  // namespace simph
#endif  // __simph_kern_Field_HPP__

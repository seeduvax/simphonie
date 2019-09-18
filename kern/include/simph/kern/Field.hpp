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
#include "Smp/IForcibleField.h"
#include "Smp/ISimpleField.h"
#include "simph/kern/Persist.hpp"
#include "simph/kern/Collection.hpp"
// workaround Smp headers issue.
namespace Smp {
class IDataflowField;
}
#include "Smp/IDataflowField.h"

namespace simph {
	namespace kern {

/**
 *
 */
class Field: public Persist, 
                    virtual public Smp::IDataflowField,
                    virtual public Smp::IForcibleField {
public:
    /**
     * Default constructor.
     */
    Field(Smp::String8 name, Smp::String8 description,
            Smp::ViewKind viewKind, void* address,
            unsigned int dataSize,
            Smp::Publication::IType* type,
            Smp::Bool isState,
            Smp::Bool isInput,
            Smp::Bool isOutput,
            Smp::IObject* parent
            );
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
        _type=type;
    }

private:
    Smp::Bool _stateType;
    Smp::Bool _inputType;
    Smp::Bool _outputType;
    const Smp::Publication::IType* _type;
    Smp::ViewKind _viewKind;
    void* _data;
    unsigned int _dataSize;
    Field* _src;
    Smp::Bool _forced;
    Smp::AnySimple _forcedValue;
    Collection<Field> _targets;
};

template <typename T>
class TField: public Field {
public:
    TField(Smp::String8 name, Smp::String8 description,
            Smp::ViewKind viewKind, T* address,
            Smp::Bool isState,
            Smp::Bool isInput,
            Smp::Bool isOutput,
            Smp::IObject* parent
            ): 
                Field(name,description,viewKind,(void*) address,sizeof(T),
                        nullptr,
                        isState,isInput,isOutput,parent),
        _tData(address) {
        initType();
    }
    virtual ~TField() {
    }
    // Smp::ISimpleField implementation
    Smp::PrimitiveTypeKind GetPrimitiveTypeKind() const override;
    Smp::AnySimple GetValue() const override;

    void SetValue(Smp::AnySimple value) {
        *_tData=value;
    }
    void initType();

private:
    T* _tData;
};

class StructureField: public Field {
public:
    StructureField(Smp::String8 name, Smp::String8 description,
            Smp::ViewKind viewKind, void* address,
            Smp::Publication::IType* type,
            Smp::Bool isState,
            Smp::Bool isInput,
            Smp::Bool isOutput,
            Smp::IObject* parent
            );
    virtual ~StructureField();
    void Push() override; 
    void Connect(Smp::IField* target) override;
    void addField(Field* f);
    inline void* getAddress() {
        return _baseAddress;
    }
    inline const std::vector<Field*> getFields() const {
        return _fields;
    } 
private:
    std::vector<Field*> _fields;
    void* _baseAddress;
};

}} // namespace simph::kern
#endif // __simph_kern_Field_HPP__

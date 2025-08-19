/*
 * @file FixedStringField.cpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simdeck/FixedStringField.hpp"
#include "Smp/IStorageReader.h"
#include "Smp/IStorageWriter.h"
#include "Smp/IOutputField.h"
#include "simdeck/Collection.hpp"

namespace simdeck {
// --------------------------------------------------------------------
// ..........................................................
FixedStringField::FixedStringField(Smp::String8 name, Smp::String8 description, Smp::ViewKind viewKind, void* address, Smp::Bool isState,
           Smp::Bool isInput, Smp::Bool isOutput, Smp::IObject* parent, const Smp::Publication::IStringType* type): Parent(name, description, viewKind, address, type->GetMaxLength(), type, isState, isInput, isOutput, parent),
    _isForced(false),
    _maxLength(type->GetMaxLength()) {
    uint64_t arraySize=_maxLength+1;
    _value=(char*)getDataPtr();
    if (address==nullptr) {
        // When address is null Field construction shold have allocated needed
        // memory then initilizing the array is needed.
        memset(_value,0,arraySize);
    }
    _forcedValue=new char[arraySize];
    memset(_forcedValue,0,arraySize);
}
// ..........................................................
FixedStringField::~FixedStringField() {
    delete[] _value;
    delete[] _forcedValue;
}

// --------------------------------------------------------------------
// ..........................................................
Smp::AnySimple FixedStringField::GetValue() const {
    return {Smp::PrimitiveTypeKind::PTK_String8,_isForced?_forcedValue:_value};
}
// ..........................................................
void FixedStringField::SetValue(Smp::AnySimple value) {
    if (value.GetType()==Smp::PrimitiveTypeKind::PTK_String8) {
        strncpy(_value,(Smp::String8)value,_maxLength);
    }
}
// ..........................................................
void FixedStringField::Force(Smp::AnySimple value) {
    if (value.GetType()==Smp::PrimitiveTypeKind::PTK_String8) {
        strncpy(_forcedValue,(Smp::String8)value,_maxLength);
        _isForced=true;
    }
}
// ..........................................................
void FixedStringField::Unforce() {
    _isForced=false;
}
// ..........................................................
void FixedStringField::Freeze() {
    strncpy(_forcedValue,_value,_maxLength);
    _isForced=true;
}
// ..........................................................
Smp::Bool FixedStringField::IsForced() const {
    return _isForced;
}
// ..........................................................
void FixedStringField::Restore(Smp::IStorageReader* reader) {
    reader->Restore(_value,_maxLength);
}
// ..........................................................
void FixedStringField::Store(Smp::IStorageWriter* writer) {
    writer->Store(_value, _maxLength);
}

// --------------------------------------------------------------------
// ..........................................................
class FixedStringOutputField: public FixedStringField, virtual public Smp::IOutputField {
public:
    
    FixedStringOutputField(Smp::String8 name, Smp::String8 description, Smp::ViewKind viewKind, void* address, Smp::Bool isState,
           Smp::Bool isInput, Smp::Bool isOutput, Smp::IObject* parent, const Smp::Publication::IStringType* type):
        FixedStringField(name, description, viewKind, address, isState,
                    isInput, isOutput, parent, type) {
    }
    virtual ~FixedStringOutputField() {
    }
    void Connect(Smp::IField* target) override {
        auto sf=dynamic_cast<Smp::ISimpleField*>(target);
        if (sf!=nullptr && !_targets.contain(sf)) {
            _targets.push_back(sf);
        }
    } 
    void Disconnect(Smp::IField* target) override {
        auto sf=dynamic_cast<Smp::ISimpleField*>(target);
        if (sf!=nullptr) {
            _targets.remove(sf);
        }
    }
    void Push() override {
        auto value=this->GetValue();
        for (auto target: _targets) {
            // TODO find a way to not have to dynamic cast since at connect
            // only ISimpleFields are inserted.
            auto sf=dynamic_cast<Smp::ISimpleField*>(target);
            sf->SetValue(value);
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
Smp::IField* FixedStringField::Create(Smp::String8 name, Smp::String8 description, Smp::ViewKind viewKind, char* address, Smp::Bool isState,
           Smp::Bool isInput, Smp::Bool isOutput, Smp::IObject* parent, const Smp::Publication::IStringType* type) {
    if (isOutput) {
        return new FixedStringOutputField(name, description, viewKind, address, isState, isInput, isOutput, parent, type);
    }
    else {
        return new FixedStringField(name, description, viewKind, address, isState, isInput, isOutput, parent, type);
    }
}

} // namespace simdeck

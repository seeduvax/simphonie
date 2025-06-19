/*
 * @file StringField.cpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simdeck/StringField.hpp"
#include "simdeck/Collection.hpp"
#include "Smp/IOutputField.h"
#include "Smp/Publication/IType.h"

#include "simdeck/StringType.hpp"

namespace simdeck {
static const Type _StringType(StringType::UuidString, Smp::PrimitiveTypeKind::PTK_String8, sizeof(std::string), "String", "C++ String");
// --------------------------------------------------------------------
// ..........................................................
StringField::StringField(Smp::String8 name,
                Smp::String8 description,
                Smp::ViewKind viewKind,
                std::string* address,
                const Smp::Publication::IType* type,
                Smp::Bool isState,
                Smp::Bool isInput,
                Smp::Bool isOutput,
                Smp::IObject* parent):
                    Parent(name, description, viewKind, address, 
                        sizeof(std::string), 
                        type == nullptr ? &_StringType : type,
                        isState, isInput, isOutput, parent) {
    _value=address;
}
// ..........................................................
StringField::~StringField() {
}
// --------------------------------------------------------------------
// ..........................................................
Smp::PrimitiveTypeKind StringField::GetPrimitiveTypeKind() const {
    return Smp::PrimitiveTypeKind::PTK_String8;
}
// ..........................................................
Smp::AnySimple StringField::GetValue() const {
    Smp::AnySimple v;
    v.SetValue(Smp::PrimitiveTypeKind::PTK_String8,
            _forced?_forcedValue.c_str():_value->c_str());
    return v;
} 
// ..........................................................
void StringField::SetValue(Smp::AnySimple value) {
    if (value.GetType()==Smp::PrimitiveTypeKind::PTK_String8) {
        *_value=(Smp::String8)value;
    }
}
// --------------------------------------------------------------------
// ..........................................................
void StringField::Force(Smp::AnySimple value) {
    if (value.GetType()==Smp::PrimitiveTypeKind::PTK_String8) {
        _forcedValue=(Smp::String8)value;
        _forced=true;
    }
}
// ..........................................................
void StringField::Unforce() {
    _forced=false;
}
// ..........................................................
Smp::Bool StringField::IsForced() const {
    return _forced;
}
// ..........................................................
void StringField::Freeze() {
    _forcedValue=*_value;
    _forced=true;
}
// ..........................................................
void StringField::Restore(Smp::IStorageReader* reader) {
    Smp::UInt64 size;
    reader->Restore(&size, sizeof(size));
    char * buf=new char[size];
    reader->Restore(buf, size);
    *_value=std::string(buf,size);
}
// ..........................................................
void StringField::Store(Smp::IStorageWriter* writer) {
    Smp::UInt64 size=_value->size();
    writer->Store(&size, sizeof(size));
    writer->Store(_value->c_str(), size);
}
// --------------------------------------------------------------------
// ..........................................................
class StringOutputField: public StringField, virtual public Smp::IOutputField {
public:
    StringOutputField(Smp::String8 name,
                Smp::String8 description,
                Smp::ViewKind viewKind,
                std::string* address,
                Smp::Publication::IType* type,
                Smp::Bool isState,
                Smp::Bool isInput,
                Smp::Bool isOutput,
                Smp::IObject* parent): StringField(name, description, viewKind,
                        address, type, isState, isInput, isOutput, parent),
                        _targets("targets","connected fields",this) {
    }
    virtual ~StringOutputField() {
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
            auto stringF=dynamic_cast<StringField*>(target);
            if (stringF!=nullptr) {
                stringF->SetValue(value);
            }
            auto simpleF=dynamic_cast<ISimpleField*>(target);
            if (simpleF!=nullptr) {
                simpleF->SetValue(value);
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
StringField* StringField::Create(Smp::String8 name,
                Smp::String8 description,
                Smp::ViewKind viewKind,
                std::string* address,
                Smp::Publication::IType* type,
                Smp::Bool isState,
                Smp::Bool isInput,
                Smp::Bool isOutput,
                Smp::IObject* parent) {
    if (isOutput) {
        return new StringOutputField(name, description, viewKind, address, type,
                isState, isInput, isOutput, parent);
    }
    else {
        return new StringField(name, description, viewKind, address, type,
                isState, isInput, isOutput, parent);
    }
}



} // namespace simdeck

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

namespace simdeck {
static Smp::Uuid _uuidString  = { 0, 0, 0, { ' ',' ','S','t','r','i','n','g'} };
static Type _stringType(_uuidString, Smp::PrimitiveTypeKind::PTK_String8, sizeof(std::string), "String", "C++ String");
// --------------------------------------------------------------------
// ..........................................................
StringField::StringField(Smp::String8 name,
                Smp::String8 description,
                Smp::ViewKind viewKind,
                std::string* address,
                Smp::Bool isState,
                Smp::Bool isInput,
                Smp::Bool isOutput,
                Smp::IObject* parent):
                    Parent(name, description, viewKind, address, 
                        sizeof(std::string), &_stringType,
                        isState, isInput, isOutput, parent) {
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
Smp::Bool StringField::IsForced() {
    return _forced;
}
// ..........................................................
void StringField::Freeze() {
    _forcedValue=*_value;
    _forced=true;
}
// --------------------------------------------------------------------
// ..........................................................
class StringOutputField: public StringField, virtual public Smp::IOutputField {
public:
    virtual ~StringOutputField() {
    }
    void Connect(Smp::IField* target) override {
        auto sf=dynamic_cast<Smp::ISimpleField*>(target);
        if (sf!=nullptr && !_targets.contain(sf)) {
            _targets.push_back(sf);
        }
    }
    void Disconnect(Smp::IField* target) {
        auto sf=dynamic_cast<Smp::ISimpleField*>(target);
        if (sf!=nullptr) {
            _targets.remove(sf);
        }
    }
    void Push() override {
        auto value=this->GetValue();
        for (auto target: _targets) {
            target->SetValue(value);
        }
    }
    const Smp::FieldCollection* GetInputFields() const override {
        return dynamic_cast<const Smp::FieldCollection*>(&_targets);
    }
    Smp::Bool IsAutomatic() const override {
        return false;
    }
private:
    Collection<Smp::ISimpleField> _targets;
};
// --------------------------------------------------------------------
// ..........................................................



} // namespace simdeck

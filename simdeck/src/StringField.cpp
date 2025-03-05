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
Smp::AnySimple StringField::GetValue() const {
    Smp::AnySimple v;
    v.SetValue(Smp::PrimitiveTypeKind::PTK_String8,_str->c_str());
    return v;
} 

} // namespace simdeck

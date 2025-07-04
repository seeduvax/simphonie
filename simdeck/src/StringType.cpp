/*
 * @file StringType.cpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simdeck/StringType.hpp"
#include "simdeck/StringField.hpp"
#include "Smp/IComponent.h"

namespace simdeck {
    
const Smp::Uuid StringType::UuidString(0, 0, 0, { 's','t','S','t','r','i','n','g'});
// --------------------------------------------------------------------
// ..........................................................
StringType::StringType(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent): 
        Parent(
                StringType::UuidString,
                Smp::PrimitiveTypeKind::PTK_String8,
                sizeof(std::string), 
                             // not really used to perform any operation on
                             // fields buffer but consistent with parent class
                             // definition.
                name, descr, parent) {
}
// ..........................................................
StringType::~StringType() {
}

// --------------------------------------------------------------------
// ..........................................................
Smp::IField* StringType::createField(
            Smp::String8 name,
            Smp::String8 description,
            Smp::IComponent* parent,
            void* address,
            Smp::ViewKind view,
            Smp::Bool state,
            Smp::Bool input,
            Smp::Bool output) const {
    return  StringField::Create(name,description,view,(std::string*)address,this,state,input,output,parent);
}
} // namespace simdeck

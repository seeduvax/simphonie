/*
 * @file SimpleVectorType.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simdeck/SimpleVectorType.hpp"
#include "simdeck/SimpleVectorField.hpp"    
#include "simdeck/Component.hpp"
#include "Smp/IObject.h"
#include <iostream>


namespace simdeck {
const Smp::Uuid SimpleVectorType::UuidVector(0, 0, 0, { 'v','e','c','t','o','r'});

// --------------------------------------------------------------------
// ..........................................................
SimpleVectorType::SimpleVectorType(Smp::PrimitiveTypeKind primitiveTypeKind, Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : Parent(SimpleVectorType::UuidVector, primitiveTypeKind, name, descr, parent) {}
// ..........................................................
SimpleVectorType::~SimpleVectorType() {}
// --------------------------------------------------------------------
// ..........................................................

Smp::IField* SimpleVectorType::createField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::IComponent* parent,
        void* address,
        Smp::ViewKind view,
        Smp::Bool state,
        Smp::Bool input,
        Smp::Bool output) const {
            std::clog << "Create simplevector field" << std::endl;
            //std::clog << "Simplevector field parent: " << parent->GetName()  << std::endl;
            return SimpleVectorField::Create(name,
            description,
            address,
            this,
            Smp::ViewKind::VK_All,
            this,
            state, 
            input,
            output,
            parent);
    }
}  // namespace simdeck

/*
 * @file VectorType.cpp
 *
 * Copyright 2026 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simdeck/VectorType.hpp"
#include "simdeck/VectorField.hpp"
#include "Smp/Publication/ITypeRegistry.h"

namespace simdeck {

// --------------------------------------------------------------------
// ..........................................................
VectorType::VectorType(Smp::Uuid uuid, Smp::Uuid puuid, Smp::PrimitiveTypeKind primitiveTypeKind, Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : Parent(uuid, primitiveTypeKind, name, descr, parent),_puuid(puuid) {}
// ..........................................................
VectorType::~VectorType() {}
// --------------------------------------------------------------------
// ..............


Smp::IField* VectorType::createField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::IComponent* parent,
        void* address,
        Smp::ViewKind view,
        Smp::Bool state,
        Smp::Bool input,
        Smp::Bool output) const {

            auto typeRegistry = dynamic_cast<Smp::Publication::ITypeRegistry*>(this->GetParent());
            auto ptype = typeRegistry->GetType(_puuid);
            return VectorField::Create(name, description, address, ptype, view, this, 
                                        state, input, output, (Smp::IObject*)parent);
            
    }

} // namespace simdeck

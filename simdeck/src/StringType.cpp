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

namespace simdeck {
    
const Smp::Uuid StringType::UuidString(0, 0, 0, { ' ',' ','S','t','r','i','n','g'});
// --------------------------------------------------------------------
// ..........................................................
StringType::StringType(Smp::IObject* parent): 
        Parent("StringType", "C++ std::string field type", parent) {
}
// ..........................................................
StringType::~StringType() {
}

// --------------------------------------------------------------------
// ..........................................................
Smp::PrimitiveTypeKind StringType::GetPrimitiveTypeKind() const {
    return Smp::PrimitiveTypeKind::PTK_String8;
}

// ..........................................................
Smp::Uuid StringType::GetUuid() const {
    return StringType::UuidString;   
}
} // namespace simdeck

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


Smp::IField* StringType::Publish(
    Smp::Publication::IPublishField* receiver,
    Smp::String8 name,
    Smp::String8 description,
    Smp::Void* address,
    Smp::ViewKind view,
    Smp::Bool state,
    Smp::Bool input,
    Smp::Bool output){
    simdeck::StringField* sf = new StringField(name,description,view,(std::string*)address,this,state,input,output,dynamic_cast<Smp::IObject*>(receiver));
    receiver->PublishField(sf);
    return sf;
}
} // namespace simdeck

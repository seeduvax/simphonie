/*
 * @file StructureType.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/StructureType.hpp"
#include "simph/kern/TypeRegistry.hpp"
#include "simph/kern/Type.hpp"
#include "simph/sys/Logger.hpp"

namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
StructureType::StructureType(Smp::Uuid uuid, TypeRegistry* typeReg,
            Smp::String8 name, Smp::String8 description, Smp::IObject* parent):
        Type(uuid,Smp::PrimitiveTypeKind::PTK_None,0,name,description,parent),
        _typeRegistry(typeReg) {
}
// ..........................................................
StructureType::~StructureType() {
}
// --------------------------------------------------------------------
// ..........................................................
void StructureType::AddField(
            Smp::String8 name, Smp::String8 description, Smp::Uuid uuid,
            Smp::Int64 offset, Smp::ViewKind view,
            Smp::Bool state, Smp::Bool input,
            Smp::Bool output) {
    Type* t=dynamic_cast<Type*>(_typeRegistry->GetType(uuid));
    if (t!=nullptr) {
        struct StructureType::FieldDescr fd;
        fd.name=name;
        fd.description=description;
        fd.uuid=uuid;
        fd.offset=offset;
        fd.view=view;
        fd.state=state;
        fd.input=input;
        fd.output=output;
        _fields.push_back(fd);
        int size=getSize();
        int newSize=offset+t->getSize();
        if (newSize>size) {
            setSize(newSize);
        }
    }
    else {
        // TODO throw somthing like InvalidType?
LOGE("Can't add type "<<name<<" to structure type "<<GetName()
<<", can't find uuid in registry.");
    }
}

}} // namespace simph::kern

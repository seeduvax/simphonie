/*
 * @file StructureType.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/smpdk/StructureType.hpp"
#include "simph/smpdk/ExTypeNotRegistered.hpp"
#include "simph/smpdk/ExInvalidType.hpp"
#include "simph/smpdk/SimpleField.hpp"
#include "simph/smpdk/Type.hpp"

#include <sstream>

namespace simph {
namespace smpdk {
// --------------------------------------------------------------------
// ..........................................................
StructureType::StructureType(Smp::Uuid uuid, Smp::Publication::ITypeRegistry* typeReg, Smp::String8 name, Smp::String8 description,
                             Smp::IObject* parent)
    : Type(uuid, Smp::PrimitiveTypeKind::PTK_None, 0, name, description, parent), _typeRegistry(typeReg) {}
// ..........................................................
StructureType::~StructureType() {}
// --------------------------------------------------------------------
// ..........................................................
void StructureType::AddField(
                Smp::String8 name,
                Smp::String8 description,
                Smp::Uuid uuid,
                Smp::UInt64 offset,
                Smp::ViewKind view,
                Smp::Bool state,
                Smp::Bool input,
                Smp::Bool output) {
    Type* t = dynamic_cast<Type*>(_typeRegistry->GetType(uuid));
    if (t != nullptr) {
// TODO from IStructureField header, type shall be checked:
//  - be a value type.
//  - not String8
        struct StructureType::FieldDescr fd;
        fd.name = name;
        fd.description = description;
        fd.uuid = uuid;
        fd.offset = offset;
        fd.size = t->getSize();
        fd.view = view;
        fd.state = state;
        fd.input = input;
        fd.output = output;
        _fields.push_back(fd);
        int size = getSize();
        int newSize = offset + fd.size;
        if (newSize > size) {
            setSize(newSize);
        }
    }
    else {
        throw new ExTypeNotRegistered(this, uuid);
    }
}
// ..........................................................
void StructureType::setup(StructureField* sf) {
    for (auto fd : _fields) {
        // TODO make better pointer arithmetic than this ugly hack to
        // make it quicly compile.
        const void* address = sf->getAddress(fd.offset);
        Type* t = dynamic_cast<Type*>(_typeRegistry->GetType(fd.uuid));
        if (t != nullptr) {
            auto f=SimpleField::Create(
                                 fd.name, 
                                 fd.description,
                                 fd.view, (Smp::Char8*)address,
                                 sf->IsState(),
                                 sf->IsInput(),
                                 sf->IsOutput(),
                                 sf,
                                 fd.uuid);
            if (f!=nullptr) {
                sf->addField(f);
            }
            else {
                std::ostringstream ss;
                ss << "invalid type for field "<<fd.name;
                throw ExInvalidType(this,ss.str().c_str());
            }
        }
        else {
            throw ExTypeNotRegistered(this,fd.uuid);
        }
    }
}

}  // namespace smpdk
}  // namespace simph

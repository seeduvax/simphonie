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
#include "simph/smpdk/Field.hpp"
#include "simph/smpdk/Type.hpp"

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
            Smp::IField* f=nullptr;
            if (fd.uuid==_char8Type.GetUuid()) {
            // TODO according fd.uuid create field using right field subclass
                f = Field::Create<Smp::Char8>(fd.name, 
                                 fd.description,
                                 fd.view, (void*)address,
                                 sf->IsState(),
                                 sf->IsInput(),
                                 sf->IsOutput(),
                                 sf);
            }
            else if (fd.uuid==_boolType.GetUuid()) {
            // TODO according fd.uuid create field using right field subclass
                f = Field::Create<Smp::Bool>(fd.name, 
                                 fd.description,
                                 fd.view, (void*)address,
                                 sf->IsState(),
                                 sf->IsInput(),
                                 sf->IsOutput(),
                                 sf);
            }
            else if (fd.uuid==_int8Type.GetUuid()) {
            // TODO according fd.uuid create field using right field subclass
                f = Field::Create<Smp::Int8>(fd.name, 
                                 fd.description,
                                 fd.view, (void*)address,
                                 sf->IsState(),
                                 sf->IsInput(),
                                 sf->IsOutput(),
                                 sf);
            }
            else if (fd.uuid==_int16Type.GetUuid()) {
            // TODO according fd.uuid create field using right field subclass
                f = Field::Create<Smp::Int16>(fd.name, 
                                 fd.description,
                                 fd.view, (void*)address,
                                 sf->IsState(),
                                 sf->IsInput(),
                                 sf->IsOutput(),
                                 sf);
            }
            else if (fd.uuid==_int32Type.GetUuid()) {
            // TODO according fd.uuid create field using right field subclass
                f = Field::Create<Smp::Int32>(fd.name, 
                                 fd.description,
                                 fd.view, (void*)address,
                                 sf->IsState(),
                                 sf->IsInput(),
                                 sf->IsOutput(),
                                 sf);
            }
            else if (fd.uuid==_int64Type.GetUuid()) {
            // TODO according fd.uuid create field using right field subclass
                f = Field::Create<Smp::Int64>(fd.name, 
                                 fd.description,
                                 fd.view, (void*)address,
                                 sf->IsState(),
                                 sf->IsInput(),
                                 sf->IsOutput(),
                                 sf);
            }
            else if (fd.uuid==_uint8Type.GetUuid()) {
            // TODO according fd.uuid create field using right field subclass
                f = Field::Create<Smp::UInt8>(fd.name, 
                                 fd.description,
                                 fd.view, (void*)address,
                                 sf->IsState(),
                                 sf->IsInput(),
                                 sf->IsOutput(),
                                 sf);
            }
            else if (fd.uuid==_uint16Type.GetUuid()) {
            // TODO according fd.uuid create field using right field subclass
                f = Field::Create<Smp::UInt16>(fd.name, 
                                 fd.description,
                                 fd.view, (void*)address,
                                 sf->IsState(),
                                 sf->IsInput(),
                                 sf->IsOutput(),
                                 sf);
            }
            else if (fd.uuid==_uint32Type.GetUuid()) {
            // TODO according fd.uuid create field using right field subclass
                f = Field::Create<Smp::UInt32>(fd.name, 
                                 fd.description,
                                 fd.view, (void*)address,
                                 sf->IsState(),
                                 sf->IsInput(),
                                 sf->IsOutput(),
                                 sf);
            }
            else if (fd.uuid==_uint64Type.GetUuid()) {
            // TODO according fd.uuid create field using right field subclass
                f = Field::Create<Smp::UInt64>(fd.name, 
                                 fd.description,
                                 fd.view, (void*)address,
                                 sf->IsState(),
                                 sf->IsInput(),
                                 sf->IsOutput(),
                                 sf);
            }
            else if (fd.uuid==_float32Type.GetUuid()) {
            // TODO according fd.uuid create field using right field subclass
                f = Field::Create<Smp::Float32>(fd.name, 
                                 fd.description,
                                 fd.view, (void*)address,
                                 sf->IsState(),
                                 sf->IsInput(),
                                 sf->IsOutput(),
                                 sf);
            }
            else if (fd.uuid==_float64Type.GetUuid()) {
            // TODO according fd.uuid create field using right field subclass
                f = Field::Create<Smp::Float64>(fd.name, 
                                 fd.description,
                                 fd.view, (void*)address,
                                 sf->IsState(),
                                 sf->IsInput(),
                                 sf->IsOutput(),
                                 sf);
            }
            if (f!=nullptr) {
                sf->addField(f);
            }
            else {
                // TODO throw exception
            }
        }
        else {
            // should not happen if type registry is well managed.
// TODO            LOGE("Can't add field " << fd.name << " to structure " << GetName() << ". Type Uuid not found in registry");
        }
    }
}

}  // namespace smpdk
}  // namespace simph

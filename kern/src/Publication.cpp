/*
 * @file Publication.cpp
 *
 * Copyright 2020 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/Publication.hpp"
#include <string.h>
#include "Smp/IEntryPointPublisher.h"
#include "simph/kern/ExInvalidFieldType.hpp"
#include "simph/kern/ExInvalidPrimitiveType.hpp"
#include "simph/kern/ExTypeNotRegistered.hpp"
#include "simph/kern/Field.hpp"
#include "simph/kern/Property.hpp"
#include "simph/kern/StructureType.hpp"
#include "simph/kern/Type.hpp"
#include "simph/smpdk/ExDuplicateName.hpp"
#include "simph/sys/Logger.hpp"
#include "simph/sys/RttiUtil.hpp"

namespace simph {
namespace kern {
// --------------------------------------------------------------------
// ..........................................................
class StructurePublication : public Publication {
public:
    StructurePublication(Smp::IObject* toPublish, Smp::Publication::ITypeRegistry* typeRegistry)
        : Publication(toPublish, typeRegistry) {}
    virtual ~StructurePublication() {}

private:
};
// --------------------------------------------------------------------
// ..........................................................
Publication::Publication(Smp::IObject* toPublish, Smp::Publication::ITypeRegistry* typeRegistry)
    : _pubObj(toPublish),
      _typeRegistry(typeRegistry),
      _fields("fields", "", this),
      _properties("properties", "", this) {
    // Smp::IEntryPointPublisher* epp = dynamic_cast<Smp::IEntryPointPublisher*>(toPublish);
    // if (epp != nullptr) {
    //    for (auto ep : *(epp->GetEntryPoints())) {
    //        addChild(ep);
    //    }
    //}
}
// ..........................................................
Publication::~Publication() {
    for (auto ch : _childs) {
        auto ep = dynamic_cast<Smp::IEntryPoint*>(ch);
        if (ch == nullptr) {
            delete ch;
        }
    }
}
// --------------------------------------------------------------------
// IObject implementation, mostly binding to the published object
// ..........................................................
Smp::String8 Publication::GetName() const {
    return _pubObj->GetName();
}
// ..........................................................
Smp::String8 Publication::GetDescription() const {
    return _pubObj->GetDescription();
}
// ..........................................................
Smp::IObject* Publication::GetParent() const {
    return _pubObj->GetParent();
}
// --------------------------------------------------------------------
// Childs management
// ..........................................................
void Publication::addChild(Smp::IObject* pub) {
    Smp::IObject* p = getChild(pub->GetName());
    if (p != nullptr) {
        throw simph::smpdk::ExDuplicateName(_pubObj, pub->GetName());
    }
    _childs.push_back(pub);
}
// ..........................................................
Smp::IObject* Publication::getChild(Smp::String8 name) const {
    for (auto ch : _childs) {
        if (strcmp(ch->GetName(), name) == 0) {
            return ch;
        }
    }
    return nullptr;
}

// --------------------------------------------------------------------
// IPublication implementation
// ..........................................................
Smp::Publication::ITypeRegistry* Publication::GetTypeRegistry() const {
    return _typeRegistry;
}
// ..........................................................
void Publication::PublishField(Smp::String8 name, Smp::String8 description, Smp::Char8* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    addField(new TField<Smp::Char8>(name, description, view, address, state, input, output, getPubObj()));
}
// ..........................................................
void Publication::PublishField(Smp::String8 name, Smp::String8 description, Smp::Bool* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    addField(new TField<Smp::Bool>(name, description, view, address, state, input, output, getPubObj()));
}
// ..........................................................
void Publication::PublishField(Smp::String8 name, Smp::String8 description, Smp::Int8* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    addField(new TField<Smp::Int8>(name, description, view, address, state, input, output, getPubObj()));
}
// ..........................................................
void Publication::PublishField(Smp::String8 name, Smp::String8 description, Smp::Int16* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    addField(new TField<Smp::Int16>(name, description, view, address, state, input, output, getPubObj()));
}
// ..........................................................
void Publication::PublishField(Smp::String8 name, Smp::String8 description, Smp::Int32* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    addField(new TField<Smp::Int32>(name, description, view, address, state, input, output, getPubObj()));
}
// ..........................................................
void Publication::PublishField(Smp::String8 name, Smp::String8 description, Smp::Int64* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    addField(new TField<Smp::Int64>(name, description, view, address, state, input, output, getPubObj()));
}
// ..........................................................
void Publication::PublishField(Smp::String8 name, Smp::String8 description, Smp::UInt8* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    addField(new TField<Smp::UInt8>(name, description, view, address, state, input, output, getPubObj()));
}
// ..........................................................
void Publication::PublishField(Smp::String8 name, Smp::String8 description, Smp::UInt16* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    addField(new TField<Smp::UInt16>(name, description, view, address, state, input, output, getPubObj()));
}
// ..........................................................
void Publication::PublishField(Smp::String8 name, Smp::String8 description, Smp::UInt32* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    addField(new TField<Smp::UInt32>(name, description, view, address, state, input, output, getPubObj()));
}
// ..........................................................
void Publication::PublishField(Smp::String8 name, Smp::String8 description, Smp::UInt64* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    addField(new TField<Smp::UInt64>(name, description, view, address, state, input, output, getPubObj()));
}
// ..........................................................
void Publication::PublishField(Smp::String8 name, Smp::String8 description, Smp::Float32* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    addField(new TField<Smp::Float32>(name, description, view, address, state, input, output, getPubObj()));
}
// ..........................................................
void Publication::PublishField(Smp::String8 name, Smp::String8 description, Smp::Float64* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    addField(new TField<Smp::Float64>(name, description, view, address, state, input, output, getPubObj()));
}
// ..........................................................
void Publication::PublishField(Smp::String8 name, Smp::String8 description, void* address, Smp::Uuid typeUuid,
                               Smp::ViewKind view, Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    Type* t = dynamic_cast<Type*>(_typeRegistry->GetType(typeUuid));
    if (t != nullptr) {
        StructureType* st = dynamic_cast<StructureType*>(t);
        if (st != nullptr) {
            addField(new StructureField(name, description, view, address, st, state, input, output, getPubObj()));
        }
        else {
            if (typeUuid == Smp::Uuids::Uuid_Bool) {
                addField(new TField<Smp::Bool>(name, description, view, (Smp::Bool*)address, state, input, output,
                                               getPubObj()));
            }
            else if (typeUuid == Smp::Uuids::Uuid_Char8) {
                addField(new TField<Smp::Char8>(name, description, view, (Smp::Char8*)address, state, input, output,
                                                getPubObj()));
            }
            else if (typeUuid == Smp::Uuids::Uuid_Int8) {
                addField(new TField<Smp::Int8>(name, description, view, (Smp::Int8*)address, state, input, output,
                                               getPubObj()));
            }
            else if (typeUuid == Smp::Uuids::Uuid_Int16) {
                addField(new TField<Smp::Int16>(name, description, view, (Smp::Int16*)address, state, input, output,
                                                getPubObj()));
            }
            else if (typeUuid == Smp::Uuids::Uuid_Int32) {
                addField(new TField<Smp::Int32>(name, description, view, (Smp::Int32*)address, state, input, output,
                                                getPubObj()));
            }
            else if (typeUuid == Smp::Uuids::Uuid_Int64) {
                addField(new TField<Smp::Int64>(name, description, view, (Smp::Int64*)address, state, input, output,
                                                getPubObj()));
            }
            else if (typeUuid == Smp::Uuids::Uuid_UInt8) {
                addField(new TField<Smp::UInt8>(name, description, view, (Smp::UInt8*)address, state, input, output,
                                                getPubObj()));
            }
            else if (typeUuid == Smp::Uuids::Uuid_UInt16) {
                addField(new TField<Smp::UInt16>(name, description, view, (Smp::UInt16*)address, state, input, output,
                                                 getPubObj()));
            }
            else if (typeUuid == Smp::Uuids::Uuid_UInt32) {
                addField(new TField<Smp::UInt32>(name, description, view, (Smp::UInt32*)address, state, input, output,
                                                 getPubObj()));
            }
            else if (typeUuid == Smp::Uuids::Uuid_UInt64) {
                addField(new TField<Smp::UInt64>(name, description, view, (Smp::UInt64*)address, state, input, output,
                                                 getPubObj()));
            }
            else if (typeUuid == Smp::Uuids::Uuid_Float32) {
                addField(new TField<Smp::Float32>(name, description, view, (Smp::Float32*)address, state, input, output,
                                                  getPubObj()));
            }
            else if (typeUuid == Smp::Uuids::Uuid_Float64) {
                addField(new TField<Smp::Float64>(name, description, view, (Smp::Float64*)address, state, input, output,
                                                  getPubObj()));
            }
            else {
                addField(
                    new Field(name, description, view, address, t->getSize(), t, state, input, output, getPubObj()));
            }
        }
    }
    else {
        std::ostringstream d;
        d << "Can't publish field " << name;
        throw ExInvalidFieldType(this, d.str().c_str());
    }
}
// ..........................................................
void Publication::PublishField(Smp::IField* field){
    LOGE("ObjectsRegistry::PublishField(Smp::IField*) not implemented yet!")}
// ..........................................................
Smp::Publication::IType* Publication::getArrayType(Smp::PrimitiveTypeKind ptk, Smp::Int64 count) {
    Type* pt = dynamic_cast<Type*>(_typeRegistry->GetType(ptk));
    if (pt == nullptr) {
        throw ExInvalidPrimitiveType(this, ptk);
    }
    Smp::Uuid arrayTypeUuid((uint32_t)(count >> 32),
                            {(uint16_t)((count & 0xffff0000) >> 16), (uint16_t)(count & 0xffff), 0},
                            pt->GetUuid().Data3);
    Smp::Publication::IType* t = _typeRegistry->GetType(arrayTypeUuid);
    if (t == nullptr) {
        std::string tname = "A";
        tname = tname + pt->GetName();
        std::string tdescr = "Array of ";
        tdescr = tdescr + pt->GetDescription();
        t = _typeRegistry->AddArrayType(tname.c_str(), tdescr.c_str(), arrayTypeUuid, pt->GetUuid(), pt->getSize(),
                                        count);
    }
    return t;
}
// ..........................................................
void Publication::PublishArray(Smp::String8 name, Smp::String8 description, Smp::Int64 count, void* address,
                               Smp::PrimitiveTypeKind type, Smp::ViewKind view, Smp::Bool state, Smp::Bool input,
                               Smp::Bool output) {
    Smp::Publication::IType* t = getArrayType(type, count);
    switch (type) {
        case Smp::PrimitiveTypeKind::PTK_Bool:
            addField(new SimpleArrayField<Smp::Bool>(name, description, count, address, type, view, t, state, input,
                                                     output, getPubObj()));
            break;
        case Smp::PrimitiveTypeKind::PTK_Char8:
            addField(new SimpleArrayField<Smp::Char8>(name, description, count, address, type, view, t, state, input,
                                                      output, getPubObj()));
            break;
        case Smp::PrimitiveTypeKind::PTK_Int8:
            addField(new SimpleArrayField<Smp::Int8>(name, description, count, address, type, view, t, state, input,
                                                     output, getPubObj()));
            break;
        case Smp::PrimitiveTypeKind::PTK_Int16:
            addField(new SimpleArrayField<Smp::Int16>(name, description, count, address, type, view, t, state, input,
                                                      output, getPubObj()));
            break;
        case Smp::PrimitiveTypeKind::PTK_Int32:
            addField(new SimpleArrayField<Smp::Int32>(name, description, count, address, type, view, t, state, input,
                                                      output, getPubObj()));
            break;
        case Smp::PrimitiveTypeKind::PTK_Int64:
            addField(new SimpleArrayField<Smp::Int64>(name, description, count, address, type, view, t, state, input,
                                                      output, getPubObj()));
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt8:
            addField(new SimpleArrayField<Smp::UInt8>(name, description, count, address, type, view, t, state, input,
                                                      output, getPubObj()));
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt16:
            addField(new SimpleArrayField<Smp::UInt16>(name, description, count, address, type, view, t, state, input,
                                                       output, getPubObj()));
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt32:
            addField(new SimpleArrayField<Smp::UInt32>(name, description, count, address, type, view, t, state, input,
                                                       output, getPubObj()));
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt64:
            addField(new SimpleArrayField<Smp::UInt64>(name, description, count, address, type, view, t, state, input,
                                                       output, getPubObj()));
            break;
        case Smp::PrimitiveTypeKind::PTK_Float32:
            addField(new SimpleArrayField<Smp::Float32>(name, description, count, address, type, view, t, state, input,
                                                        output, getPubObj()));
            break;
        case Smp::PrimitiveTypeKind::PTK_Float64:
            addField(new SimpleArrayField<Smp::Float64>(name, description, count, address, type, view, t, state, input,
                                                        output, getPubObj()));
            break;
        default:
            LOGE("Unsupported array base type " << type);
            break;
    }
}
// ..........................................................
Smp::IPublication* Publication::PublishArray(Smp::String8 name, Smp::String8 description, Smp::ViewKind view,
                                             Smp::Bool state) {
    LOGE("Publication::PublishArray(...) not implemented yet!")
    return nullptr;
}
// ..........................................................
Smp::IPublication* Publication::PublishStructure(Smp::String8 name, Smp::String8 description, Smp::ViewKind view,
                                                 Smp::Bool state) {
    LOGE("Publication::PublishStructure(...) not implemented yet!")
    return nullptr;
}
// ..........................................................
Smp::Publication::IPublishOperation* Publication::PublishOperation(Smp::String8 name, Smp::String8 description,
                                                                   Smp::ViewKind view) {
    LOGE("Publication::PublishOperation(...) not implemented yet!")
    return nullptr;
}
// ..........................................................
void Publication::PublishProperty(Smp::String8 name, Smp::String8 description, Smp::Uuid typeUuid,
                                  Smp::AccessKind accessKind, Smp::ViewKind view) {
    Smp::Publication::IType* type = _typeRegistry->GetType(typeUuid);
    if (type != nullptr) {
        // TODO use this or wrapped obj as parent?
        // TODO are only simple type OK for properties?
        Property* p = new Property(name, description, this, type, accessKind, view);
        _properties.push_back(p);
        addChild(p);
    }
    else {
        throw ExTypeNotRegistered(this, typeUuid);
    }
}
// ..........................................................
void Publication::Unpublish(){// TODO
                              LOGE("Publication::Unpublish(...) not implemented yet!")}
// ..........................................................
Smp::IField* Publication::GetField(Smp::String8 fullName) const {
    return _fields.at(fullName);
}
// ..........................................................
const Smp::FieldCollection* Publication::GetFields() const {
    return &_fields;
}
const Smp::PropertyCollection* Publication::GetProperties() const {
    return &_properties;
}
const Smp::OperationCollection* Publication::GetOperations() const {
    // TODO
    return nullptr;
}
// ..........................................................
Smp::IRequest* Publication::CreateRequest(Smp::String8 operationName) {
    LOGE("Publication::CreateRequest(...) not implemented yet!")
    return nullptr;
}
// ..........................................................
void Publication::DeleteRequest(Smp::IRequest* request) {
    LOGE("Publication::DeleteRequest(...) not implemented yet!")
}

// ..........................................................
void Publication::dump(int level) {
    for (int i = 0; i < level; ++i) {
        std::cout << "    ";
    }
    std::cout << GetName() << ": " << simph::sys::RttiUtil::getTypeName(_pubObj) << std::endl;
    for (auto child : _childs) {
        auto sp = dynamic_cast<Publication*>(child);
        if (sp != nullptr) {
            sp->dump(level + 1);
        }
        else {
            for (int i = 0; i < level; ++i) {
                std::cout << "    ";
            }
            std::cout << "  " << child->GetName() << ": " << simph::sys::RttiUtil::getTypeName(child) << std::endl;
        }
    }
}

}  // namespace kern
}  // namespace simph

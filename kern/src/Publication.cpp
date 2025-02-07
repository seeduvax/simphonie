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
#include "Smp/IComponent.h"
#include "Smp/IEntryPointPublisher.h"
#include "Smp/Publication/IArrayType.h"
#include "simph/smpdk/ExInvalidPrimitiveType.hpp"
#include "simph/smpdk/ExTypeNotRegistered.hpp"
#include "simph/smpdk/Field.hpp"
#include "simph/smpdk/Property.hpp"
#include "simph/smpdk/StructureType.hpp"
#include "simph/smpdk/Type.hpp"
#include "simph/smpdk/ExDuplicateName.hpp"
#include "simph/sys/Logger.hpp"
#include "simph/sys/RttiUtil.hpp"

namespace simph {
namespace kern {

using namespace simph::smpdk;
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
    : _pubObj(dynamic_cast<Smp::IComponent*>(toPublish)),
      _typeRegistry(typeRegistry),
      _properties("properties", "", this) {
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
// ..........................................................
Smp::IObject* Publication::GetChild(Smp::String8 name) const {
    return _pubObj->GetChild(name);
}
// --------------------------------------------------------------------
// Childs management
// TODO to be reconsidered since IObject now have GetChild and
// IComponent have AddChild etc.
// ..........................................................
void Publication::addChild(Smp::IObject* pub) {
    Smp::IObject* p = getChild(pub->GetName());
    if (p != nullptr) {
        throw simph::smpdk::ExDuplicateName(_pubObj, pub->GetName());
    }
    _childs.push_back(pub);
}
void Publication::addField(Smp::IField* field) {
    auto* f = _pubObj->GetChild(field->GetName());
    if (f != nullptr) {
        throw simph::smpdk::ExDuplicateName(_pubObj, field->GetName());
    }
    _pubObj->AddChild(field,(Smp::ICollectionBase*)_pubObj->GetFields());
}
// ..........................................................
Smp::IObject* Publication::getChild(Smp::String8 name) const {
    for (auto ch : _childs) {
        if (strcmp(ch->GetName(), name) == 0) {
            return ch;
        }
    }
    return _pubObj->GetChild(name);
}

// --------------------------------------------------------------------
// IPublication implementation
// ..........................................................
Smp::Publication::ITypeRegistry* Publication::GetTypeRegistry() const {
    return _typeRegistry;
}
// ..........................................................
Smp::IField* Publication::PublishField(Smp::String8 name, Smp::String8 description, Smp::Char8* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    auto f=SimpleField::Create(name, description, view, address, state, input, output, _pubObj);
    addField(f);
    return f;
}
// ..........................................................
Smp::IField* Publication::PublishField(Smp::String8 name, Smp::String8 description, Smp::Bool* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    auto f=SimpleField::Create(name, description, view, address, state, input, output, _pubObj);
    addField(f);
    return f;
}
// ..........................................................
Smp::IField* Publication::PublishField(Smp::String8 name, Smp::String8 description, Smp::Int8* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    auto f=SimpleField::Create(name, description, view, address, state, input, output, _pubObj);
    addField(f);
    return f;
}
// ..........................................................
Smp::IField* Publication::PublishField(Smp::String8 name, Smp::String8 description, Smp::Int16* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    auto f=SimpleField::Create(name, description, view, address, state, input, output, _pubObj);
    addField(f);
    return f;
}
// ..........................................................
Smp::IField* Publication::PublishField(Smp::String8 name, Smp::String8 description, Smp::Int32* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    auto f=SimpleField::Create(name, description, view, address, state, input, output, _pubObj);
    addField(f);
    return f;
}
// ..........................................................
Smp::IField* Publication::PublishField(Smp::String8 name, Smp::String8 description, Smp::Int64* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    auto f=SimpleField::Create(name, description, view, address, state, input, output, _pubObj);
    addField(f);
    return f;
}
// ..........................................................
Smp::IField* Publication::PublishField(Smp::String8 name, Smp::String8 description, Smp::UInt8* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    auto f=SimpleField::Create(name, description, view, address, state, input, output, _pubObj);
    addField(f);
    return f;
}
// ..........................................................
Smp::IField* Publication::PublishField(Smp::String8 name, Smp::String8 description, Smp::UInt16* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    auto f=SimpleField::Create(name, description, view, address, state, input, output, _pubObj);
    addField(f);
    return f;
}
// ..........................................................
Smp::IField* Publication::PublishField(Smp::String8 name, Smp::String8 description, Smp::UInt32* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    auto f=SimpleField::Create(name, description, view, address, state, input, output, _pubObj);
    addField(f);
    return f;
}
// ..........................................................
Smp::IField* Publication::PublishField(Smp::String8 name, Smp::String8 description, Smp::UInt64* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    auto f=SimpleField::Create(name, description, view, address, state, input, output, _pubObj);
    addField(f);
    return f;
}
// ..........................................................
Smp::IField* Publication::PublishField(Smp::String8 name, Smp::String8 description, Smp::Float32* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    auto f=SimpleField::Create(name, description, view, address, state, input, output, _pubObj);
    addField(f);
    return f;
}
// ..........................................................
Smp::IField* Publication::PublishField(Smp::String8 name, Smp::String8 description, Smp::Float64* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    auto f=SimpleField::Create(name, description, view, address, state, input, output, _pubObj);
    addField(f);
    return f;
}
// ..........................................................
Smp::IField* Publication::PublishField(Smp::String8 name, Smp::String8 description, void* address, Smp::Uuid typeUuid,
                               Smp::ViewKind view, Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    Type* t = dynamic_cast<Type*>(_typeRegistry->GetType(typeUuid));
    if (t != nullptr) {
        StructureType* st = dynamic_cast<StructureType*>(t);
        if (st != nullptr) {
/* TODO restore structure field build.
            auto f=new StructureField(name, description, view, address, st, state, input, output, _pubObj);
            addField(f);
            return f;
*/
return nullptr;
        }
        else {
            auto f=SimpleField::Create(name, 
                                       description, 
                                       view, 
                                       address,
                                       state,
                                       input,
                                       output,
                                       _pubObj,
                                       typeUuid);
            if (f!=nullptr) {
                addField(f);
                return f;
            }
            else {
/*
                auto f=
                    new Field(name, description, view, address, t->getSize(), t, state, input, output, _pubObj);
                addField(f);
                return f;
*/
            }
        }
    }
    else {
        std::ostringstream d;
        d << "Can't publish field " << name;
// TODO find what exception to throw here.
//        throw XXXXX(this, d.str().c_str());
    }
    return nullptr;
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
Smp::ISimpleArrayField* Publication::PublishArray(Smp::String8 name, Smp::String8 description, Smp::Int64 count, void* address,
                               Smp::PrimitiveTypeKind type, Smp::ViewKind view, Smp::Bool state, Smp::Bool input,
                               Smp::Bool output) {
TRACE("!!!!!!!!!!!!!")
    Smp::Publication::IType* t = getArrayType(type, count);
    auto* rf=SimpleArrayField::Create(
                                    name,
                                    description,
                                    count,
                                    address,
                                    type,
                                    view,
                                    t,
                                    state,
                                    input,
                                    output,
                                    _pubObj);
    if (rf!=nullptr) {
        addField(rf);
    }
    return rf;
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
Smp::Publication::IPublishOperation* Publication::PublishOperation(
                    Smp::String8 name,
                    Smp::String8 description,
                    Smp::ViewKind view) {
    // TODO
    LOGE("Publication::PublishOperation(...) not implemented yet!")
    return nullptr;
}
// ..........................................................
void Publication::PublishOperation(Smp::IOperation* operation) {
    // TODO
    LOGE("Publication::PublishOperation(...) not implemented yet!")
}
// ..........................................................
Smp::IProperty* Publication::PublishProperty(
                        Smp::String8 name,
                        Smp::String8 description,
                        Smp::Uuid typeUuid,
                        Smp::AccessKind accessKind,
                        Smp::ViewKind view) {
    Smp::Publication::IType* type = _typeRegistry->GetType(typeUuid);
    if (type != nullptr) {
        // TODO use this or wrapped obj as parent?
        // TODO are only simple type OK for properties?
        Property* p = new Property(name, description, this, type, accessKind, view);
        _properties.push_back(p);
        addChild(p);
        return p;
    }
    else {
        throw ExTypeNotRegistered(this, typeUuid);
    }
    return nullptr;
}
// ..........................................................
void Publication::PublishProperty(Smp::IProperty* property) {
    _properties.push_back(property);
    addChild(property);
}
// ..........................................................
Smp::IProperty* Publication::GetProperty(Smp::String8 name) const {
    for (auto p: _properties) {
        if (strcmp(name,p->GetName())==0) {
            return p;
        }
    }
    return nullptr;
}
// ..........................................................
void Publication::Unpublish(){// TODO
                              LOGE("Publication::Unpublish(...) not implemented yet!")}
// ..........................................................
Smp::IField* Publication::GetField(Smp::String8 fullName) const {
    return _pubObj->GetField(fullName);
}
// ..........................................................
const Smp::FieldCollection* Publication::GetFields() const {
    return _pubObj->GetFields();
}
const Smp::PropertyCollection* Publication::GetProperties() const {
    return &_properties;
}
Smp::IOperation* Publication::GetOperation(Smp::String8 name) const {
    // TODO
    return nullptr;
}
const Smp::OperationCollection* Publication::GetOperations() const {
    // TODO
    return nullptr;
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

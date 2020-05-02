/*
 * @file ObjectsRegistry.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/ObjectsRegistry.hpp"
#include "simph/kern/Field.hpp"
#include "simph/kern/Type.hpp"
#include "simph/kern/StructureType.hpp"
#include "simph/kern/ExInvalidPrimitiveType.hpp"
#include "simph/sys/Logger.hpp"
#include "simph/sys/RttiUtil.hpp"
#include "Smp/IEntryPoint.h"
#include <regex>


namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
class Node {
public:
    Node(Smp::IObject* toWrap, Node* parent):
            _wrapped(toWrap),
            _parent(parent) {
        if (_parent!=nullptr) {
// TODO check for duplicate names
            _parent->_children.push_back(this);
        }
    }
    inline Smp::IObject* getObject() const {
        return _wrapped;
    }

    inline Node* getParent() const {
        return _parent;
    }
    inline Node* get(const Smp::IObject *o) {
        for (auto n: _children) {
            if (n->_wrapped==o) {
                return n;
            }
        }
        return nullptr;
    }
    inline const std::vector<Node*>* getChildren() {
        return &_children;
    }
    inline Node* get(const Smp::String8 name) {
        for (auto n: _children) {
            if (strcmp(n->_wrapped->GetName(),name)==0) {
                return n;
            }
        }
        return nullptr;
    }
    void dump(int level) {
        if (_wrapped!=nullptr) {
            for (int i=0;i<level;++i) {
                std::cout << "    ";
            }
            std::cout << _wrapped->GetName() << ": " 
                << simph::sys::RttiUtil::getTypeName(_wrapped) << std::endl;
        }
        for (auto child: _children) {
            child->dump(level+1);
        }
    }
    inline const std::vector<Smp::IDataflowField*>* getFlowFields() {
        return &_flowFields;
    }
    inline void addField(Smp::IField* field) {
        auto flowField=dynamic_cast<Smp::IDataflowField*>(field);
        if (flowField!=nullptr && flowField->IsOutput()) {
            _flowFields.push_back(flowField);
        }
    }
private:
    Smp::IObject* _wrapped;
    Node* _parent;
    std::vector<Node*> _children;
    std::vector<Smp::IDataflowField*> _flowFields;
};
// --------------------------------------------------------------------
// ..........................................................
ObjectsRegistry::ObjectsRegistry(Smp::String8 name,
                            Smp::String8 descr,
                            Smp::IObject* parent,
                            TypeRegistry* typeRegistry):
                    Component(name,descr,parent),
    _root(new Node(nullptr,nullptr)),
    _currentNode(_root),
    _typeRegistry(typeRegistry) {
}
// ..........................................................
ObjectsRegistry::~ObjectsRegistry() {
}
// ..........................................................
void ObjectsRegistry::dump(Smp::IObject* from) {
    Node* n=findNode(from);
    if (n==nullptr) {
        n=_root;
    }
    n->dump(0);
}
// --------------------------------------------------------------------
// ..........................................................
Smp::IObject* ObjectsRegistry::resolve(Node* from,Smp::String8 path) {
    Smp::IObject* res=nullptr;
    Node* n=from;
    if (n==nullptr) {
        n=_root;
    }
    std::string input=path;
    std::regex re("/");
    const std::sregex_token_iterator end;
    for (std::sregex_token_iterator it{input.begin(), input.end(), re, -1};
                            it!=end && n!=nullptr;++it) {
        std::string name=*it;
        if (name=="..") {
            n=n->getParent();
        }
        else {
            n=n->get(name.c_str());
        }
    }
    if (n!=nullptr) {
        res=n->getObject();
    }
    return res;
}
// ..........................................................
Smp::IObject* ObjectsRegistry::ResolveAbsolute(Smp::String8 absolutePath) {
    return resolve(_root,absolutePath);
}
// ..........................................................
Smp::IObject* ObjectsRegistry::ResolveRelative(
                                    Smp::String8 relativePath,
                                    const Smp::IComponent* sender) {
    Smp::IObject* res=nullptr;
    Node* n=findNode(sender);
    if (n!=nullptr) {
        res=resolve(n,relativePath);
    }
    else {
        LOGE("Requested relative path search from unregistered object: "
                << sender->GetName());
    }
    return res;
}
// ..........................................................
Smp::IObject* ObjectsRegistry::getFieldParent() {
    Smp::IObject* res=nullptr;
    if (_currentNode!=nullptr) {
        res=_currentNode->getObject();
    }
    return res;
}
// ..........................................................
void ObjectsRegistry::add(Smp::IObject* obj) {
    Node* parent=findNode(obj->GetParent());
    _currentNode=new Node(obj,parent);
}
// ..........................................................
Node* ObjectsRegistry::findNode(const Smp::IObject* obj) {
    Node* res=_root;
    if (obj!=nullptr) {
        Node* p=findNode(obj->GetParent());
        if (p!=nullptr) {
            res=p->get(obj);
        }
    }
    return res;;
}
// --------------------------------------------------------------------
// ..........................................................
const std::vector<Smp::IDataflowField*>* ObjectsRegistry::getRelatedFlowFields(
                                                const Smp::IEntryPoint* ep) {
    const std::vector<Smp::IDataflowField*>* res=nullptr;
    Node* n=findNode(ep->GetParent());
    if (n!=nullptr) {
        res=n->getFlowFields();
    }
    return res;
}
// ..........................................................
void ObjectsRegistry::addField(Smp::IField* field) {
    Node* newNode=new Node(field,findNode(field->GetParent()));
    _currentNode->addField(field);
    StructureField* sf=dynamic_cast<StructureField*>(field);
    if (sf!=nullptr) {
        Node* savedCurrent=_currentNode;
        _currentNode=newNode;
        for(auto subF: sf->getFields()) {
            addField(subF);
        }
        _currentNode=savedCurrent;
    }
}
// ..........................................................
Smp::Publication::ITypeRegistry* ObjectsRegistry::GetTypeRegistry() const {
    return _typeRegistry;
}
// ..........................................................
void ObjectsRegistry::PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::Char8* address,
        Smp::ViewKind view,
        Smp::Bool state,
        Smp::Bool input,
        Smp::Bool output) {
    addField(new TField<Smp::Char8>(name,description,view,address,
                    state,input,output,getFieldParent()));
}
// ..........................................................
void ObjectsRegistry::PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::Bool* address,
        Smp::ViewKind view,
        Smp::Bool state,
        Smp::Bool input,
        Smp::Bool output) {
    addField(new TField<Smp::Bool>(name,description,view,address,
                    state,input,output,getFieldParent()));
}
// ..........................................................
void ObjectsRegistry::PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::Int8* address,
        Smp::ViewKind view,
        Smp::Bool state,
        Smp::Bool input,
        Smp::Bool output) {
    addField(new TField<Smp::Int8>(name,description,view,address,
                    state,input,output,getFieldParent()));
}
// ..........................................................
void ObjectsRegistry::PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::Int16* address,
        Smp::ViewKind view,
        Smp::Bool state,
        Smp::Bool input,
        Smp::Bool output) {
    addField(new TField<Smp::Int16>(name,description,view,address,
                    state,input,output,getFieldParent()));
}
// ..........................................................
void ObjectsRegistry::PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::Int32* address,
        Smp::ViewKind view,
        Smp::Bool state,
        Smp::Bool input,
        Smp::Bool output) {
    addField(new TField<Smp::Int32>(name,description,view,address,
                    state,input,output,getFieldParent()));
}
// ..........................................................
void ObjectsRegistry::PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::Int64* address,
        Smp::ViewKind view,
        Smp::Bool state,
        Smp::Bool input,
        Smp::Bool output) {
    addField(new TField<Smp::Int64>(name,description,view,address,
                    state,input,output,getFieldParent()));
}
// ..........................................................
void ObjectsRegistry::PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::UInt8* address,
        Smp::ViewKind view,
        Smp::Bool state,
        Smp::Bool input,
        Smp::Bool output) {
    addField(new TField<Smp::UInt8>(name,description,view,address,
                    state,input,output,getFieldParent()));
}
// ..........................................................
void ObjectsRegistry::PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::UInt16* address,
        Smp::ViewKind view,
        Smp::Bool state,
        Smp::Bool input,
        Smp::Bool output) {
    addField(new TField<Smp::UInt16>(name,description,view,address,
                    state,input,output,getFieldParent()));
}
// ..........................................................
void ObjectsRegistry::PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::UInt32* address,
        Smp::ViewKind view,
        Smp::Bool state,
        Smp::Bool input,
        Smp::Bool output) {
    addField(new TField<Smp::UInt32>(name,description,view,address,
                    state,input,output,getFieldParent()));
}
// ..........................................................
void ObjectsRegistry::PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::UInt64* address,
        Smp::ViewKind view,
        Smp::Bool state,
        Smp::Bool input,
        Smp::Bool output) {
    addField(new TField<Smp::UInt64>(name,description,view,address,
                    state,input,output,getFieldParent()));
}
// ..........................................................
void ObjectsRegistry::PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::Float32* address,
        Smp::ViewKind view,
        Smp::Bool state,
        Smp::Bool input,
        Smp::Bool output) {
    addField(new TField<Smp::Float32>(name,description,view,address,
                    state,input,output,getFieldParent()));
}
// ..........................................................
void ObjectsRegistry::PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::Float64* address,
        Smp::ViewKind view,
        Smp::Bool state,
        Smp::Bool input,
        Smp::Bool output) {
    addField(new TField<Smp::Float64>(name,description,view,address,
                    state,input,output,getFieldParent()));
}
// ..........................................................
void ObjectsRegistry::PublishField(
        Smp::String8 name,
        Smp::String8 description,
        void* address,
        Smp::Uuid typeUuid,
        Smp::ViewKind view,
        Smp::Bool state,
        Smp::Bool input,
        Smp::Bool output) {
    Type* t=dynamic_cast<Type*>(_typeRegistry->GetType(typeUuid));
    if (t!=nullptr) {
        StructureType* st=dynamic_cast<StructureType*>(t);
        if (st!=nullptr) {
            addField(new StructureField(name,description,view,
                        address, st,state, input, output,
                        getFieldParent()));
        }
        else {
            // TODO should probably do a switch case of death to ensure
            // proper field subclass is create. May be there is already
            // one somewhere...
            addField(new Field(name,description,view,address,t->getSize(),t,
                    state,input,output,getFieldParent()));
        }    
    }
    else {
LOGE("No type found or bad type. Can't add field "<<name<<" to "
        <<getFieldParent()->GetName());
        // TODO throw the right exception...
    }
}
// ..........................................................
void ObjectsRegistry::PublishField(Smp::IField* field) {
LOGE("ObjectsRegistry::PublishField(Smp::IField*) not implemented yet!")
}
// ..........................................................
Smp::Publication::IType* ObjectsRegistry::getArrayType(Smp::PrimitiveTypeKind ptk,
                                                        Smp::Int64 count) {
    Type* pt=dynamic_cast<Type*>(_typeRegistry->GetType(ptk)); 
    if (pt==nullptr) {
        throw ExInvalidPrimitiveType(this,ptk);
    }
    Smp::Uuid arrayTypeUuid(
            (uint32_t)(count>>32),
            {(uint16_t)((count&0xffff0000)>>16),
            (uint16_t)(count&0xffff),
            0},
            pt->GetUuid().Data3);
    Smp::Publication::IType* t=_typeRegistry->GetType(arrayTypeUuid);
    if (t==nullptr) {
        std::string tname="A";
        tname=tname+pt->GetName();
        std::string tdescr="Array of ";
        tdescr=tdescr+pt->GetDescription();
        t=_typeRegistry->AddArrayType(tname.c_str(),tdescr.c_str(),
                arrayTypeUuid,pt->GetUuid(),pt->getSize(),count);
    }
    return t;
}
// ..........................................................
void ObjectsRegistry::PublishArray(
        Smp::String8 name,
        Smp::String8 description,
        Smp::Int64 count,
        void* address,
        Smp::PrimitiveTypeKind type,
        Smp::ViewKind view,
        Smp::Bool state,
        Smp::Bool input,
        Smp::Bool output) {
    Smp::Publication::IType* t=getArrayType(type,count);
    switch (type) {
        case Smp::PrimitiveTypeKind::PTK_Bool:
            addField(new SimpleArrayField<Smp::Bool>(name,description,
                    count,address,type,
                    view,t,state,input,output,getFieldParent()));
            break;
        case Smp::PrimitiveTypeKind::PTK_Char8:
           addField(new SimpleArrayField<Smp::Char8>(name,description,
                    count,address,type,
                    view,t,state,input,output,getFieldParent()));
            break;
        case Smp::PrimitiveTypeKind::PTK_Int8:
           addField(new SimpleArrayField<Smp::Int8>(name,description,
                    count,address,type,
                    view,t,state,input,output,getFieldParent()));
            break;
        case Smp::PrimitiveTypeKind::PTK_Int16:
           addField(new SimpleArrayField<Smp::Int16>(name,description,
                    count,address,type,
                    view,t,state,input,output,getFieldParent()));
            break;
        case Smp::PrimitiveTypeKind::PTK_Int32:
           addField(new SimpleArrayField<Smp::Int32>(name,description,
                    count,address,type,
                    view,t,state,input,output,getFieldParent()));
            break;
        case Smp::PrimitiveTypeKind::PTK_Int64:
           addField(new SimpleArrayField<Smp::Int64>(name,description,
                    count,address,type,
                    view,t,state,input,output,getFieldParent()));
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt8:
           addField(new SimpleArrayField<Smp::UInt8>(name,description,
                    count,address,type,
                    view,t,state,input,output,getFieldParent()));
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt16:
           addField(new SimpleArrayField<Smp::UInt16>(name,description,
                    count,address,type,
                    view,t,state,input,output,getFieldParent()));
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt32:
           addField(new SimpleArrayField<Smp::UInt32>(name,description,
                    count,address,type,
                    view,t,state,input,output,getFieldParent()));
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt64:
           addField(new SimpleArrayField<Smp::UInt64>(name,description,
                    count,address,type,
                    view,t,state,input,output,getFieldParent()));
            break;
        case Smp::PrimitiveTypeKind::PTK_Float32:
           addField(new SimpleArrayField<Smp::Float32>(name,description,
                    count,address,type,
                    view,t,state,input,output,getFieldParent()));
            break;
        case Smp::PrimitiveTypeKind::PTK_Float64:
           addField(new SimpleArrayField<Smp::Float64>(name,description,
                    count,address,type,
                    view,t,state,input,output,getFieldParent()));
            break;
        default:
            LOGE("Unsupported array base type "<<type);
            break;
    }
}
// ..........................................................
Smp::IPublication* ObjectsRegistry::PublishArray(
        Smp::String8 name,
        Smp::String8 description,
        Smp::ViewKind view,
        Smp::Bool state) {
LOGE("ObjectsRegistry::PublishArray(...) not implemented yet!")
return nullptr;
}
// ..........................................................
Smp::IPublication* ObjectsRegistry::PublishStructure(
        Smp::String8 name,
        Smp::String8 description,
        Smp::ViewKind view,
        Smp::Bool state) {
LOGE("ObjectsRegistry::PublishStructure(...) not implemented yet!")
return nullptr;
}
// ..........................................................
Smp::Publication::IPublishOperation* ObjectsRegistry::PublishOperation(
        Smp::String8 name,
        Smp::String8 description,
        Smp::ViewKind view) {
LOGE("ObjectsRegistry::PublishOperation(...) not implemented yet!")
return nullptr;
}
// ..........................................................
void ObjectsRegistry::PublishProperty(
        Smp::String8 name,
        Smp::String8 description,
        Smp::Uuid typeUuid,
        Smp::AccessKind accessKind,
        Smp::ViewKind view) {
LOGE("ObjectsRegistry::PublishProperty(...) not implemented yet!")
}
// ..........................................................
void ObjectsRegistry::Unpublish() {
// TODO
LOGE("ObjectsRegistry::PublishProperty(...) not implemented yet!")
}
// ..........................................................
Smp::IField* ObjectsRegistry::GetField(Smp::String8 fullName) const {
    return Component::GetField(fullName);
}
// ..........................................................
const Smp::FieldCollection* ObjectsRegistry::GetFields() const {
    return Component::GetFields();
}
const Smp::PropertyCollection* ObjectsRegistry::GetProperties() const {
// TODO
    return nullptr;
}
const Smp::OperationCollection* ObjectsRegistry::GetOperations() const {
// TODO
    return nullptr;
}
// ..........................................................
Smp::IRequest* ObjectsRegistry::CreateRequest(Smp::String8 operationName) {
LOGE("ObjectsRegistry::CreateRequest(...) not implemented yet!")
return nullptr;
}
// ..........................................................
void ObjectsRegistry::DeleteRequest(Smp::IRequest* request) {
LOGE("ObjectsRegistry::DeleteRequest(...) not implemented yet!")
}

}} // namespace simph::kern

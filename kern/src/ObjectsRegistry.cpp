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
    _currentNode(nullptr),
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
// TODO unsure Fields should be attached directly to component or
// field container. May be it should be an option...
    Smp::IObject* res=nullptr;
    if (_currentNode!=nullptr) {
        for (auto child: *(_currentNode->getChildren())) {
            if (dynamic_cast<Smp::FieldCollection*>(child->getObject())) {
                return child->getObject();
            }
        }
        res=_currentNode->getObject();
    }
    return res;
}
// ..........................................................
void ObjectsRegistry::add(Smp::IObject* obj) {
    Node* parent=findNode(obj->GetParent());
    _currentNode=new Node(obj,parent);
    auto c=dynamic_cast<Smp::IComponent*>(obj); 
    if (c!=nullptr && c->GetFields()!=nullptr) {
        new Node((Smp::IObject*)c->GetFields(),_currentNode);
    }
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
    new Node(field,findNode(field->GetParent()));
    _currentNode->addField(field);
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
        addField(new Field(name,description,view,address,t->getSize(),
                    state,input,output,getFieldParent()));
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
LOGE("ObjectsRegistry::PublishArray(...,void*,...) not implemented yet!")
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
Smp::IField* ObjectsRegistry::GetField(Smp::String8 fullName) const {
    return Component::GetField(fullName);
}
// ..........................................................
const Smp::FieldCollection* ObjectsRegistry::GetFields() const {
    return Component::GetFields();
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

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
#include "simph/sys/Logger.hpp"
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
        if (_parent!=NULL) {
            _parent->_childs.push_back(this);
        }
    }
    inline Smp::IObject* getObject() const {
        return _wrapped;
    } 
    inline Node* getParent() const {
        return _parent;
    }
    inline Node* get(const Smp::IObject *o) {
        for (auto n: _childs) {
            if (n->_wrapped==o) {
                return n;
            }
        }
        return nullptr;
    }
    inline Node* get(const Smp::String8 name) {
        for (auto n: _childs) {
            if (strcmp(n->_wrapped->GetName(),name)==0) {
                return n;
            }
        }
        return nullptr;
    }
    void dump(int level) {
        for (int i;i<level;++i) {
            std::cout << "  ";
        }
        std::cout << _wrapped->GetName() << std::endl;
        for (auto child: _childs) {
            child->dump(level+1);
        }
    }
private:
    Smp::IObject* _wrapped;
    Node* _parent;
    std::vector<Node*> _childs;
};
// --------------------------------------------------------------------
// ..........................................................
ObjectsRegistry::ObjectsRegistry(Smp::String8 name, Smp::String8 descr,
                            Smp::IObject* parent):
                    Component(name,descr,parent) {
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
void ObjectsRegistry::addField(Smp::IField* field) {
    new Node(field,_currentNode);
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
                    state,input,output,_currentNode->getObject()));
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
                    state,input,output,_currentNode->getObject()));
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
                    state,input,output,_currentNode->getObject()));
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
                    state,input,output,_currentNode->getObject()));
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
                    state,input,output,_currentNode->getObject()));
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
                    state,input,output,_currentNode->getObject()));
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
                    state,input,output,_currentNode->getObject()));
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
                    state,input,output,_currentNode->getObject()));
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
                    state,input,output,_currentNode->getObject()));
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
                    state,input,output,_currentNode->getObject()));
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
                    state,input,output,_currentNode->getObject()));
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
                    state,input,output,_currentNode->getObject()));
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
LOGE("ObjectsRegistry::PublishField(...,void*,...) not implemented yet!")
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
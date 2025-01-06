/*
 * @file Resolver.cpp
 *
 * Copyright 2020 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/Resolver.hpp"
#include "Smp/IArrayField.h"
#include "Smp/IComposite.h"
#include "Smp/IComponent.h"
#include "Smp/ISimpleField.h"
#include "Smp/ISimulator.h"
#include "Smp/IModel.h"
#include "Smp/IService.h"
#include "Smp/Publication/IType.h"
#include "simph/kern/Publication.hpp"
#include "simph/sys/Callback.hpp"
#include "simph/sys/Logger.hpp"

namespace simph {
namespace kern {
// --------------------------------------------------------------------
// ..........................................................
Resolver::Resolver(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : simph::smpdk::Component(name, descr, parent) {
    // Default root is the parent object, until connect step where
    // the simulator is recovered.
    _root=parent;
    addEP("dump", "List published objects to stdout", &Resolver::dump, this);
}
// ..........................................................
Resolver::~Resolver() {
}
// --------------------------------------------------------------------
// ..........................................................
void Resolver::connect() {
    _root=getSimulator();
}
// --------------------------------------------------------------------
// ..........................................................
Smp::IObject* Resolver::resolve(Smp::String8 path, Smp::IObject* from) {
    Smp::IObject* res = nullptr;
    Smp::IObject* obj = from;
    std::string input = path;

    std::string slashInput = input.substr(0, input.find("/"));

    int dotIdx = slashInput.find(".");
    std::string dotInput = slashInput.substr(0, dotIdx);

    std::string bracketInput1 = dotInput.substr(0, dotInput.find("["));

    int bracketIdx2 = bracketInput1.find("]");
    std::string bracketInput2 = bracketInput1.substr(0, bracketIdx2);

    // .. case
    if (dotIdx == 0 && char(input[1]) == '.') {
        obj = obj->GetParent();

        // ... or more dot case delete only first dot
        if (char(input[2]) == '.') {
            input = input.substr(1, -1);
        }
        // ../ case
        else if (char(input[2]) == '/') {
            input = input.substr(3, -1);
        }
        // .. case
        else {
            input = input.substr(2, -1);
        }
    }
    // Bracket case
    else if (bracketIdx2 != -1) {
        // get field
        auto field = dynamic_cast<Smp::IArrayField*>(from);
        if (field != nullptr) {
            obj = field->GetItem(std::stoi(bracketInput2));
        }
        res = obj;

        // delete next delimiter if it's not the last element of the path
        input = input.size() > bracketInput2.size() ? input.substr(bracketInput2.size() + 1) : "";
    }
    // commone object child case
    else {
        // get child object;
        obj = obj->GetChild(bracketInput1.c_str());

        // delete next delimiter if it's not the last element of the path
        input = input.size() > bracketInput1.size() ? input.substr(bracketInput1.size() + 1) : "";
        res=obj;
    }

    if (input.size() > 0 && obj != nullptr) {
        res = resolve(input.c_str(), obj);
    }
    // stop if the path is incorrect
    else if (input.size() > 0) {
        res = nullptr;
    }

    return res;
}

// ..........................................................
Smp::IObject* Resolver::ResolveAbsolute(Smp::String8 absolutePath) {
    return resolve(absolutePath, _root);
}
// ..........................................................
Smp::IObject* Resolver::ResolveRelative(Smp::String8 relativePath, Smp::IObject* sender) {
    return resolve(relativePath, sender);
}
// --------------------------------------------------------------------
// ..........................................................
void Resolver::dump() const {
    dumpObj(_root);
}
// ..........................................................
void Resolver::dumpObj(const Smp::IObject* from, int level) const {
    for (int i=0; i<level; i++) {
        std::cout << "    ";
    }
    std::cout << from->GetName() << " ";
    if (dynamic_cast<const Smp::IService*>(from)!=nullptr) {
        std::cout << "[Service]";
    }   
    if (dynamic_cast<const Smp::IModel*>(from)!=nullptr) {
        std::cout << "[Model]";
    }   
    if (dynamic_cast<const Smp::IField*>(from)!=nullptr) {
        auto f=dynamic_cast<const Smp::IField*>(from);
        std::cout << " [Field";
        if (f->IsInput()) {
            std::cout << ":in";
        }
        if (f->IsOutput()) {
            std::cout << ":out";
        }
        std::cout << ":" << f->GetType()->GetPrimitiveTypeKind() << "]";
    }
    if (dynamic_cast<const Smp::ISimulator*>(from)!=nullptr) {
        std::cout << "[Simulator]";
    }
    if (dynamic_cast<const Smp::IContainer*>(from)!=nullptr) {
        std::cout << "[Container]";
    }
    if (dynamic_cast<const Smp::IComposite*>(from)!=nullptr) {
        std::cout << "[Composite]";
    }
    if (dynamic_cast<const Smp::IEntryPoint*>(from)!=nullptr) {
        std::cout << "[EntryPoint]";
    }
    int l=level+1;
    std::cout << std::endl;
    auto ctnr=dynamic_cast<const Smp::IContainer*>(from);
    if (ctnr!=nullptr) {
        for (auto cp: *(ctnr->GetComponents())) {
            dumpObj(cp,l);
        }
    }
    auto cpst=dynamic_cast<const Smp::IComposite*>(from);
    if (cpst!=nullptr) {
        for (auto ct: *(cpst->GetContainers())) {
            dumpObj(ct,l);
        }
    }
    auto epp=dynamic_cast<const Smp::IEntryPointPublisher*>(from);
    if (epp!=nullptr) {
        for (auto ep: *(epp->GetEntryPoints())) {
            dumpObj(ep,l);
        } 
    }   
    auto c=dynamic_cast<const Smp::IComponent*>(from);
    if (c!=nullptr) {
        for (auto f: *(c->GetFields())) {
            dumpObj(f,l);
        } 
    }   
}

}  // namespace kern
}  // namespace simph

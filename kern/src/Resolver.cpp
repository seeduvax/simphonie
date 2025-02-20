/*
 * @file Resolver.cpp
 *
 * Copyright 2020 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/kern/Resolver.hpp"
#include "Smp/IArrayField.h"
#include "Smp/IComposite.h"
#include "Smp/IComponent.h"
#include "Smp/ISimpleField.h"
#include "Smp/ISimulator.h"
#include "Smp/IModel.h"
#include "Smp/IService.h"
#include "Smp/Publication/IType.h"
#include "simphonie/kern/Publication.hpp"
#include "simphonie/sys/Callback.hpp"
#include "simphonie/sys/Logger.hpp"

namespace simphonie {
namespace kern {
// --------------------------------------------------------------------
// ..........................................................
Resolver::Resolver(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : simdeck::Component(name, descr, parent) {
    // Default root is the parent object, until connect step where
    // the simulator is recovered.
    _root=parent;
    addEP("dump", "List published objects to stdout", this, &Resolver::dump);
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
Smp::IObject* Resolver::ResolveAbsolute(Smp::String8 absolutePath) {
    return resolveChild(absolutePath, _root);
}
// ..........................................................
Smp::IObject* Resolver::ResolveRelative(Smp::String8 relativePath, Smp::IObject* sender) {
    return resolveChild(relativePath, sender);
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

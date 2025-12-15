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

#include "Smp/IComponent.h"
#include "Smp/IComposite.h"
#include "Smp/IModel.h"
#include "Smp/IOutputField.h"
#include "Smp/IService.h"
#include "Smp/ISimpleArrayField.h"
#include "Smp/ISimpleField.h"
#include "Smp/ISimulator.h"
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
    _linkeRegistry = getSimulator()->GetLinkRegistry();
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
std::string Resolver::getFullName(Smp::IObject* o) const {
    if (o == _root || o == nullptr) {
        return "";
    }
    else {
        return getFullName(o->GetParent()) + "/" + o->GetName();
    }
}
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
        auto t = f->GetType();
        if (t != nullptr) {
            std::cout << ":" << f->GetType()->GetPrimitiveTypeKind();
        }
        auto af = dynamic_cast<const Smp::ISimpleArrayField*>(from);
        if (af != nullptr) {
            std::cout << "[" << af->GetSize() << "]";
        }
        std::cout << "]";
        auto of=dynamic_cast<const Smp::IOutputField*>(f);
        if (of!=nullptr) {
            for (auto in: *(of->GetInputFields())) {
                std::cout << std::endl;
                for (int i=0; i<level; i++) {
                    std::cout << "    ";
                }
                std::cout << "    -> " << getFullName(in);
            }
        }
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
        for (auto source : *(_linkeRegistry->GetLinkSources(c))) {
            for (int i = 0; i < level; i++) {
                std::cout << "    ";
            }
            std::cout << "    <-[" << _linkeRegistry->GetLinkCount(source, c) << "]- " << getFullName(source)
                      << std::endl;
        }
        for (auto f: *(c->GetFields())) {
            dumpObj(f,l);
        } 
    }
}

}  // namespace kern
}  // namespace simph

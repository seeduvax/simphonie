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
#include "simph/kern/Publication.hpp"
#include "simph/sys/Logger.hpp"
#include "Smp/IComposite.h"
#include "Smp/ISimulator.h"
#include <regex>

namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
Resolver::Resolver(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent):
                    simph::smpdk::Component(name,descr,parent) {
    // Parent should be a Simulator holong already a TypeRegistry...
    _typeRegistry=nullptr;
    auto c=dynamic_cast<Smp::IComposite*>(parent);
    if (c!=nullptr) {
        _typeRegistry=dynamic_cast<Smp::Publication::ITypeRegistry*>(
                c->GetContainer(Smp::ISimulator::SMP_SimulatorServices)
                 ->GetComponent("TypeRegistry"));
    }
    _root=new Publication(parent,_typeRegistry);
}
// ..........................................................
Resolver::~Resolver() {
    delete _root;
}
// --------------------------------------------------------------------
// ..........................................................
Smp::IObject* Resolver::resolve(Smp::String8 path, Publication* from) {
    Smp::IObject* res=nullptr;
    Publication* p=from;
    std::string input=path;
    std::regex re("/");
    const std::sregex_token_iterator end;
    for (std::sregex_token_iterator it{input.begin(), input.end(), re, -1};
                            it!=end && p!=nullptr;++it) {
        std::string name=*it;
        if (name=="..") {
            auto it=_publications.find(p->GetParent());
            p=it==_publications.end()?nullptr:it->second;
        }
        else {
            p=dynamic_cast<Publication*>(p->getChild(name.c_str()));
        }
    }
    if (p!=nullptr) {
        res=p->getPubObj();
    }
    return res;
}
// ..........................................................
Smp::IObject* Resolver::ResolveAbsolute(Smp::String8 absolutePath) {
    return resolve(absolutePath,_root);
}
// ..........................................................
Smp::IObject* Resolver::ResolveRelative(Smp::String8 relativePath,
                        const Smp::IComponent* sender) {
    Smp::IObject* res=nullptr;
    auto it=_publications.find(sender);
    if (it!=_publications.end()) {
        res=resolve(relativePath,it->second);
    }
    return res;
}
// --------------------------------------------------------------------
// ..........................................................
Smp::IPublication* Resolver::publish(Smp::IObject* toPublish) {
    Publication* pub=nullptr;
    auto itPub=_publications.find(toPublish);
    if (itPub==_publications.end()) {
        pub=new Publication(toPublish,_typeRegistry);
        _publications[toPublish]=pub;
        Publication* parentPub=_root;
        Smp::IObject* parent=toPublish->GetParent();
        if (parent!=nullptr) {
            auto itParent=_publications.find(parent);
            if (itParent!=_publications.end()) {
                parentPub=itParent->second;
            }
            else {
                LOGW("Publication of "<<toPublish->GetName()
                    <<"'s parent not found. Publishing object to root node.");
            }
        }
        parentPub->addChild(pub);
    }
    else {
        pub=itPub->second;
    }
    return pub;
}

}} // namespace simph::kern

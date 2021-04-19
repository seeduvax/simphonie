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
#include <regex>
#include "Smp/IComposite.h"
#include "Smp/ISimulator.h"
#include "simph/kern/Publication.hpp"
#include "simph/sys/Callback.hpp"
#include "simph/sys/Logger.hpp"

namespace simph {
namespace kern {
// --------------------------------------------------------------------
// ..........................................................
Resolver::Resolver(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : simph::smpdk::Component(name, descr, parent) {
    // Parent should be a Simulator holong already a TypeRegistry...
    _typeRegistry = nullptr;
    auto c = dynamic_cast<Smp::IComposite*>(parent);
    if (c != nullptr) {
        _typeRegistry = dynamic_cast<Smp::Publication::ITypeRegistry*>(
            c->GetContainer(Smp::ISimulator::SMP_SimulatorServices)->GetComponent("TypeRegistry"));
    }
    if (parent != nullptr) {
        _root = new Publication(parent, _typeRegistry);
        _publications[parent] = _root;
    }
    else {
        _root = new Publication(this, _typeRegistry);
        _publications[this] = _root;
    }
    addEP(std::move(simph::sys::Callback::create(&Resolver::dump, this)), "dump", "List published objects to stdout");
}
// ..........................................................
Resolver::~Resolver() {
    delete _root;
}
// --------------------------------------------------------------------
// ..........................................................
Smp::IObject* Resolver::resolve(Smp::String8 path, Publication* from) {
    Smp::IObject* res = nullptr;
    Publication* p = from;
    std::string input = path;
    std::regex re("/");  // re("\b(?:([.])(?!\1))+\b|[/]") / single . or /
    const std::sregex_token_iterator end;
    for (std::sregex_token_iterator it{input.begin(), input.end(), re, -1}; it != end && p != nullptr; ++it) {
        res = nullptr;
        if (p != nullptr) {
            std::string name = *it;
            Smp::IObject* o = nullptr;  // object published by current publication 'p'
            if (name == "..") {
                auto it = _publications.find(p->GetParent());
                p = it == _publications.end() ? nullptr : it->second;
                res = p->getPubObj();
            }
            else {
                o = p->getChild(name.c_str());
                if (o != nullptr) {
                    p = dynamic_cast<Publication*>(o);
                    // a field has a dedicated publication
                    // an entryppoint has no dedicated publication
                    res = p != nullptr ? p->getPubObj() : o;
                }
                else {
                    p = nullptr;
                    res = nullptr;
                }
            }
            // TODO support '[]' operator on structure field
        }
    }
    return res;
}
// ..........................................................
Smp::IObject* Resolver::ResolveAbsolute(Smp::String8 absolutePath) {
    return resolve(absolutePath, _root);
}
// ..........................................................
Smp::IObject* Resolver::ResolveRelative(Smp::String8 relativePath, const Smp::IComponent* sender) {
    Smp::IObject* res = nullptr;
    auto it = _publications.find(sender);
    if (it != _publications.end()) {
        res = resolve(relativePath, it->second);
    }
    return res;
}
// --------------------------------------------------------------------
// ..........................................................
void Resolver::dump() {
    _root->dump();
}
// ..........................................................
Smp::IPublication* Resolver::publish(Smp::IObject* toPublish) {
    Publication* pub = nullptr;
    auto itPub = _publications.find(toPublish);
    if (itPub == _publications.end()) {
        pub = new Publication(toPublish, _typeRegistry);
        _publications[toPublish] = pub;
        Publication* parentPub = _root;
        Smp::IObject* parent = toPublish->GetParent();
        if (parent != nullptr) {
            auto itParent = _publications.find(parent);
            if (itParent != _publications.end()) {
                parentPub = itParent->second;
            }
            else {
                LOGW("Publication of " << toPublish->GetName()
                                       << "'s parent not found. Publishing object to root node.");
            }
        }
        parentPub->addChild(pub);
    }
    else {
        pub = itPub->second;
    }
    return pub;
}
// ..........................................................
const Smp::IPublication* Resolver::getPublication(const Smp::IObject* obj) const {
    auto it = _publications.find(obj);
    return it != _publications.end() ? it->second : nullptr;
}

}  // namespace kern
}  // namespace simph

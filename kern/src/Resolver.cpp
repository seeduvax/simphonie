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
#include "Smp/IPublication.h"
#include "Smp/ISimpleField.h"
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
    if (_typeRegistry == nullptr) {
        throw std::runtime_error("Cannot find 'TypeRegistry'");
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
        Publication* p = dynamic_cast<Publication*>(obj);

        auto it = _publications.find(p->GetParent());
        p = it == _publications.end() ? nullptr : it->second;
        res = p != nullptr ? p->getPubObj() : nullptr;
        obj = dynamic_cast<Smp::IObject*>(p);

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
    // Publication case
    else {
        Publication* p = dynamic_cast<Publication*>(obj);
        // get child object;
        obj = p->getChild(bracketInput1.c_str());
        if (obj != nullptr) {
            p = dynamic_cast<Publication*>(obj);
            // a field has a dedicated publication
            // an entryppoint has no dedicated publication
            res = p != nullptr ? p->getPubObj() : obj;
        }
        else {
            res = nullptr;
        }

        // delete next delimiter if it's not the last element of the path
        input = input.size() > bracketInput1.size() ? input.substr(bracketInput1.size() + 1) : "";
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

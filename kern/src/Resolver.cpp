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
Smp::IObject* Resolver::resolve(Smp::String8 path, Publication* from) {
    Smp::IObject* res = nullptr;
    Publication* p = from;
    std::string input = path;

    std::string slashInput = input.substr(0, input.find("/"));

    int dotIdx = slashInput.find(".");
    std::string dotInput = slashInput.substr(0, dotIdx);

    std::string bracketInput1 = dotInput.substr(0, dotInput.find("["));

    int bracketIdx2 = bracketInput1.find("]");
    std::string bracketInput2 = bracketInput1.substr(0, bracketIdx2);

    // .. case
    if (dotIdx == 0 && char(input[1]) == '.') {
        std::cout << input[0] << input[1] << input[2] << std::endl;

        auto it = _publications.find(p->GetParent());
        p = it == _publications.end() ? nullptr : it->second;
        res = p != nullptr ? p->getPubObj() : nullptr;

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
        std::cout << "Bracket : " << bracketInput2 << " " << dotIdx << std::endl;
        // delete first character ( suppose to be '[' )
        bracketInput2.erase(0, 1);

        // get field
        auto field = dynamic_cast<Smp::IArrayField*>(from);

        if (field != nullptr) {
            res = field->GetItem(std::stoi(bracketInput2));
        }

        // input = input.substr(bracketIdx2, -1);
        // delete [XX].   +2 for []. characters
        // input = bracketIdx2 != -1 ? input.substr(bracketIdx2, -1) : input.substr(bracketInput.size()+3,-1);
        input = input.substr(input.find_first_not_of(bracketInput2) + 1);
        // TODO to rework delete potential serparator after bracket
        if (char(input[0] == '.') || char(input[0] == '/')) {
            input = input.substr(2);
        }
    }
    // Publication case
    else {
        std::cout << "Publication : " << dotInput << " " << dotIdx << " " << &p << std::endl;
        // get child object;
        Smp::IObject* o = p->getChild(dotInput.c_str());
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

        // input = bracketIdx2 != -1 ? input.substr(bracketIdx2, -1) : input.substr(dotInput.size(),-1);
        input = input.substr(dotInput.size());
        // TODO to rework delete potential serparator after bracket
        if (char(input[0] == '.') || char(input[0] == '/')) {
            input = input.substr(1);
        }
    }

    if (input.size() > 0) {
        simph::kern::Publication* child = dynamic_cast<simph::kern::Publication*>(res);
        res = resolve(input.c_str(), child);
    }
    /*
    std::vector<std::string> SlashPath = HashString(input, "/");
    // Hash slash delimiters
    for (std::string itr : SlashPath) {
        Smp::IObject* o = nullptr;
        // Exclude double dot delimiter (get parent)
        if (itr == "..") {
            auto it = _publications.find(p->GetParent());
            p = it == _publications.end() ? nullptr : it->second;
            res = p != nullptr ? p->getPubObj() : nullptr;
        }
        else {
            // Hash dot delimiters
            std::vector<std::string> DotPath = HashString(itr, "[.]");
            for (std::string itr2 : DotPath) {
                std::vector<std::string> BracketPath = HashString(itr2, "[[]");
                // detect if we're between brackets
                bool open = false;
                for (std::string itr3 : BracketPath) {
                    // clean useless ] at the end
                    itr3.erase(remove(itr3.begin(), itr3.end(), ']'), itr3.end());

                    if (open == true) {
                        // get field
                        auto field = dynamic_cast<Smp::IArrayField*>(o);

                        if (field != nullptr) {
                            o = field->GetItem(std::stoi(itr3));
                            res = o;
                        }

                        // int simph::kern::ArrayField
                    }
                    else {
                        // get child object;
                        o = p->getChild(itr3.c_str());
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

                        open = true;
                    }
                }
            }
        }
    }
    */

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

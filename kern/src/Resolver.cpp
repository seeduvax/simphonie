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
    // See path parsing state diagram in definition file (ref 1e2889e) 
    // subsection "Path parsing" somewhere in "Design" section.
    // (fig resolver_path_parse_state.png)
    Smp::IObject* obj=from;
    enum State {
        Init,
        Sep,
        Self,
        Parent,
        Name,
        Index,
        EndIndex,
        DotSep,
        End 
    };
    State state=State::Init;
    int i=0;
    std::string name="";
    while (state!=State::End) {
        char c=path[i];
        switch (state) {
            case State::Init: {
                    switch (c) {
                        case '/':
                            state=State::Sep;
                            break;
                        case '.':
                            state=State::Self;
                            break;
                        default:
                            // don't really care char validity, since this
                            // should lead to not found child and finally 
                            // returning nullptr.
                            // This will be the same for many case of switching
                            // to Name state and won't be repeated further
                            name.append(1,c);
                            state=State::Name;
                            break;
                    }
                }
                break;
            case State::Sep: {
                    switch (c) {
                        case '/':
                            // Ignore succesive /
                            break;
                        case '.':
                            state=State::Self;
                            break;
                        default:
                            name.append(1,c);
                            state=State::Name;
                            break;
                    }
                }
                break;
            case State::Self: {
                    switch (c) {
                        case '/':
                            state=State::Sep;
                            break;
                        case '.':
                            state=State::Parent;
                            break;
                        default:
                            name.append(1,c);
                            state=State::Name;
                    }
                }
                break;
            case State::Parent: {
                    switch(c) {
                        case '/':
                            obj=obj->GetParent();
                            state=State::Sep;
                            break;
                        case '\0':
                            obj=obj->GetParent();
                            state=State::End;
                            break;
                        default:
                            obj=nullptr;
                            state=State::End;
                    }
                }
                break;
            case State::Name: {
                    switch (c) {
                        case '/':
                            obj=obj->GetChild(name.c_str());
                            name="";
                            state=State::Sep;
                            break;
                        case '[':
                            obj=obj->GetChild(name.c_str());
                            name="[";
                            state=State::Index;
                            break;
                        case '.':
                            obj=obj->GetChild(name.c_str());
                            name="";
                            state=State::DotSep;
                            break;
                        case '\0':
                            obj=obj->GetChild(name.c_str());
                            state=State::End;
                            break;
                        default:
                            name.append(1,c);
                            break;
                    }
                }
                break;
            case State::Index: {
                    switch (c) {
                        case ']':
                            name.append(1,c);
                            obj=obj->GetChild(name.c_str());
                            name="";
                            state=State::EndIndex;
                            break;
                        default:
                            name.append(1,c);
                            break;
                    }
                }
                break;
            case State::EndIndex: {
                    switch (c) {
                        case '/':
                            state=State::Sep;
                            break;
                        case '.':
                            state=State::DotSep;
                            break;
                        case '[':
                            state=State::Index;
                            break;
                        case '\0':
                            state=State::End;
                            break;
                        default:
                            // parse error
                            obj=nullptr;
                            break;
                    }
                }
                break;
            case State::DotSep: {
                    switch (c) {
                        case '.':
                        case '/':
                            // parse error
                            obj=nullptr;
                            break;
                        default:
                            name.append(1,c);
                            state=State::Name;
                    }
                }
                break;
        }
        if (c=='\0' || obj==nullptr) {
            // force final state if end of string is reached or at some level
            // any of GetParent() or GetChild() returned nullptr.
            state=State::End;
        }
        else {
            // jump to next char.
            i++;
        }
    }
    return obj;
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

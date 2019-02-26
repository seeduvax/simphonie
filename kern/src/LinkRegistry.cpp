/*
 * @file LinkRegistry.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/LinkRegistry.hpp"
#include "Smp/ILinkingComponent.h"

namespace Smp {
    namespace Services {
constexpr Smp::Char8 ILinkRegistry::SMP_LinkRegistry[];
}}

namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
LinkRegistry::LinkRegistry(Smp::String8 name, Smp::String8 descr,
                                Smp::IObject* parent):
            Component(name[0]=='\0'?Smp::Services::ILinkRegistry::SMP_LinkRegistry:name,descr,parent){
}
// ..........................................................
LinkRegistry::~LinkRegistry() {
    for (auto p: _links) {
        // may be ILinkingComponent::RemoveLinks shall be invoke when possible
        // for each link. But when we are here, it is because Simulator is
        // destroying. So, is it really needed to care about remaining links?
        delete p.second;
    }
}
// --------------------------------------------------------------------
// ..........................................................
void LinkRegistry::AddLink(Smp::IComponent* source,
            const Smp::IComponent* target) {
    auto it=_links.find(target);
    if (it==_links.end()) {
        _links[target]=new Collection<Smp::IComponent>(target->GetName(),
                                "Links",this);
    }
    _links[target]->push_back(source);
}
// ..........................................................
Smp::Bool LinkRegistry::HasLink(const Smp::IComponent* source,
                                const Smp::IComponent* target) {
    auto it=_links.find(target);
    if (it!=_links.end()) {
        for (auto cmp: *(it->second)) {
            if (cmp==source) {
                return true;
            }
        }
    }
    return false;
}
// ..........................................................
void LinkRegistry::RemoveLink(const Smp::IComponent* source,
                    const Smp::IComponent* target) {
    auto it=_links.find(target);
    if (it!=_links.end()) {
        it->second->remove((Smp::IComponent*)source);
    }
}
// ..........................................................
const Smp::ComponentCollection* LinkRegistry::GetLinkSources(
                    const Smp::IComponent* target) const {
    Smp::ComponentCollection* res=nullptr;
    auto it=_links.find(target);
    if (it!=_links.end()) {
        res=it->second;
    }
    return res;
}
// ..........................................................
Smp::Bool LinkRegistry::CanRemove(const Smp::IComponent* target) {
    auto it=_links.find(target);
    if (it!=_links.end()) {
        for (auto cmp: *(it->second)) {
            if (dynamic_cast<Smp::ILinkingComponent*>(cmp)==nullptr) {
                return false;
            }
        }
    }
    return true;
}
// ..........................................................
void LinkRegistry::RemoveLinks(const Smp::IComponent* target) {
    auto it=_links.find(target);
    if (it!=_links.end()) {
        for (auto cmp: *(it->second)) {
            Smp::ILinkingComponent* lcmp=dynamic_cast<Smp::ILinkingComponent*>(cmp);
            if (lcmp!=nullptr) {
                lcmp->RemoveLinks(target);
            }
        }
        delete it->second;
        _links.erase(it);
    }
}

}} // namespace simph::kern

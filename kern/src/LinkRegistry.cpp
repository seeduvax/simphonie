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
            Component(name[0]=='\0'?Smp::Services::ILinkRegistry::SMP_LinkRegistry:name,descr,parent),
            _links() {
}
// ..........................................................
LinkRegistry::~LinkRegistry() {
}
// --------------------------------------------------------------------
// ..........................................................
void LinkRegistry::AddLink(Smp::IComponent* source,
            const Smp::IComponent* target) {
    auto it = _links.emplace(std::piecewise_construct,
                std::forward_as_tuple(target),
                std::forward_as_tuple(target->GetName(),"Links",this));
    it.first->second.push_back(source);
}
// ..........................................................
Smp::UInt32 LinkRegistry::GetLinkCount(const Smp::IComponent* source,
                                const Smp::IComponent* target) const {
    Smp::UInt32 count=0;
    auto it=_links.find(target);
    if (it!=_links.end()) {
        for (auto t: it->second) {
            if (t==target) {
                count++;
            }
        }
    }
    return count;;
}
// ..........................................................
Smp::Bool LinkRegistry::RemoveLink(const Smp::IComponent* source,
                    const Smp::IComponent* target) {
    auto it=_links.find(target);
    if (it!=_links.end()) {
        it->second.remove(source);
        return true;
    }
    return false;
}
// ..........................................................
const Smp::ComponentCollection* LinkRegistry::GetLinkSources(
                    const Smp::IComponent* target) const {
    auto it=_links.find(target);
    return it!=_links.end() ? &it->second:nullptr;
}
// ..........................................................
Smp::Bool LinkRegistry::CanRemove(const Smp::IComponent* target) {
    bool canRemove = true;
    auto it=_links.find(target);
    if (it!=_links.end()) {
        for (auto src: it->second) {
            if (dynamic_cast<Smp::ILinkingComponent*>(src)==nullptr) {
                canRemove = false;
                break;
            }
        }
    }
    return canRemove;
}
// ..........................................................
void LinkRegistry::RemoveLinks(const Smp::IComponent* target) {
    auto it=_links.find(target);
    if (it!=_links.end()) {
        for (auto src: it->second) {
            Smp::ILinkingComponent* lsrc=dynamic_cast<Smp::ILinkingComponent*>(src);
            if (lsrc!=nullptr) {
                lsrc->RemoveLinks(target);
            }
        }
        _links.erase(it);
    }
}

}} // namespace simph::kern

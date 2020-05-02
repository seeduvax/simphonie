/*
 * @file LinkRegistry.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_LinkRegistry_HPP__
#define __simph_kern_LinkRegistry_HPP__
#include "Smp/Services/ILinkRegistry.h"
#include "simph/smpdk/Component.hpp"
#include "simph/smpdk/Collection.hpp"
#include <unordered_map>

namespace simph {
	namespace kern {
using namespace simph::smpdk;

/**
 *
 */
class LinkRegistry: public Component,
            virtual public Smp::Services::ILinkRegistry {
public:
    /**
     * Default constructor.
     */
    LinkRegistry(Smp::String8 name=Smp::Services::ILinkRegistry::SMP_LinkRegistry,
            Smp::String8 descr="",
            Smp::IObject* parent=nullptr);
    /**
     * Destructor.
     */
    virtual ~LinkRegistry();
    // Smp::Services::ILinkRegistry implementation
    void AddLink(Smp::IComponent* source,
                const Smp::IComponent* target) override;
    Smp::UInt32 GetLinkCount(const Smp::IComponent* source,
                const Smp::IComponent* target) const override;
    Smp::Bool RemoveLink(Smp::IComponent* source,
                const Smp::IComponent* target) override;
    const Smp::ComponentCollection* GetLinkSources(
                const Smp::IComponent* target) const override;
    Smp::Bool CanRemove(const Smp::IComponent* target) override;
    void RemoveLinks(const Smp::IComponent* target) override;
private:
    // may be ILinkingComponent::RemoveLinks shall be invoke when possible
    // for each link. But when we are at destroying this, it is because Simulator is
    // destroying. So, is it really needed to care about remaining links?
    // (AA) Consider unordered_multimap for this containership ?
    typedef std::unordered_map<const Smp::IComponent*,Collection<Smp::IComponent>> LinkMap;
    LinkMap _links;
};

}} // namespace simph::kern
#endif // __simph_kern_LinkRegistry_HPP__

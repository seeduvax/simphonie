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
#include "simph/kern/Component.hpp"
#include "simph/kern/Collection.hpp"
#include <map>

namespace simph {
	namespace kern {

/**
 *
 */
class LinkRegistry: virtual public Component,
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
                const Smp::IComponent* target);
    Smp::Bool HasLink(const Smp::IComponent* source,
                const Smp::IComponent* target);
    void RemoveLink(const Smp::IComponent* source,
                const Smp::IComponent* target);
    const Smp::ComponentCollection* GetLinkSources(
                const Smp::IComponent* target) const;
    Smp::Bool CanRemove(const Smp::IComponent* target);
    void RemoveLinks(const Smp::IComponent* target);
private:
    std::map<const Smp::IComponent*,Collection<Smp::IComponent>*> _links;
};

}} // namespace simph::kern
#endif // __simph_kern_LinkRegistry_HPP__

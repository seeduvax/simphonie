/*
 * @file Resolver.h
 *
 * Copyright 2020 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_Resolver_HPP__
#define __simph_kern_Resolver_HPP__

#include "Smp/Services/IResolver.h"
#include "Smp/Publication/ITypeRegistry.h"
#include "simph/smpdk/Component.hpp"
#include "simph/kern/Publication.hpp"
#include "simph/kern/IEntryPointPublisher.hpp"
#include <map>

namespace simph {
	namespace kern {
class Publication;

/**
 *
 */
class Resolver: public simph::smpdk::Component,
            virtual public Smp::Services::IResolver,
            virtual public IEntryPointPublisher {
public:
    /**
     * Default constructor.
     */
    Resolver(Smp::String8 name, Smp::String8 descr="", 
                            Smp::IObject* parent=nullptr);
    /**
     * Destructor.
     */
    virtual ~Resolver();

    // Smp::Services::IResolver implementation
    Smp::IObject* ResolveAbsolute(Smp::String8 absolutePath) override;
    Smp::IObject* ResolveRelative(Smp::String8 relativePath,
                                const Smp::IComponent* sender) override;

    Smp::IPublication* publish(Smp::IObject* toPublish);

    void dump();
private:
    Publication* _root;
    std::map<const Smp::IObject*,Publication*> _publications;
    Smp::Publication::ITypeRegistry* _typeRegistry;
    Smp::IObject* resolve(Smp::String8 path, Publication* from);
};

}} // namespace simph::kern
#endif // __simph_kern_Resolver_HPP__

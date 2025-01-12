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

#include <map>
#include <regex>
#include "Smp/Publication/ITypeRegistry.h"
#include "Smp/Services/IResolver.h"
#include "simph/smpdk/IEntryPointPublisher.hpp"
#include "simph/kern/Publication.hpp"
#include "simph/smpdk/Component.hpp"

namespace simph {
namespace kern {
class Publication;

/**
 *
 */
class Resolver : public simph::smpdk::Component,
                 virtual public Smp::Services::IResolver,
                 virtual public simph::smpdk::IEntryPointPublisher {
public:
    /**
     * Default constructor.
     */
    Resolver(Smp::String8 name, Smp::String8 descr = "", Smp::IObject* parent = nullptr);
    /**
     * Destructor.
     */
    virtual ~Resolver();

    // Smp::Services::IResolver implementation
    Smp::IObject* ResolveAbsolute(Smp::String8 absolutePath) override;
    Smp::IObject* ResolveRelative(Smp::String8 relativePath, Smp::IObject* sender) override;

    void dump() const;

protected:
    void connect();

private:
    Smp::IObject* _root;
    Smp::IObject* resolve(Smp::String8 path, Smp::IObject* from);
    void dumpObj(const Smp::IObject* from, int level=0) const;
};

}  // namespace kern
}  // namespace simph
#endif  // __simph_kern_Resolver_HPP__

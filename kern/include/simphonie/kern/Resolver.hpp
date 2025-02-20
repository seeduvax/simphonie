/*
 * @file Resolver.h
 *
 * Copyright 2020 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_kern_Resolver_HPP__
#define __simphonie_kern_Resolver_HPP__

#include <map>
#include <regex>
#include "Smp/Publication/ITypeRegistry.h"
#include "Smp/Services/IResolver.h"
#include "simdeck/EntryPointPublisher.hpp"
#include "simphonie/kern/Publication.hpp"
#include "simdeck/Component.hpp"

namespace simphonie {
namespace kern {
class Publication;

/**
 *
 */
class Resolver : public simdeck::Component,
                 virtual public Smp::Services::IResolver,
                 virtual public simdeck::EntryPointPublisher {
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
    void connect() override;

private:
    Smp::IObject* _root;
    void dumpObj(const Smp::IObject* from, int level=0) const;
};

}  // namespace kern
}  // namespace simph
#endif  // __simphonie_kern_Resolver_HPP__

/*
 * @file EntryPointPublisher.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_EntryPointPublisher_HPP__
#define __simdeck_EntryPointPublisher_HPP__

#include "Smp/IEntryPointPublisher.h"
#include "simdeck/EntryPoint.hpp"
#include "simdeck/Collection.hpp"

namespace simdeck {
using namespace simdeck;

class EntryPointPublisher : virtual public Smp::IEntryPointPublisher {
public:
    virtual ~EntryPointPublisher();

    template <typename Owner, typename Func>
    inline Smp::IEntryPoint* addEP(Smp::String8 name, Smp::String8 descr , Owner owner, Func f) {
        auto ep=new TEntryPoint<Owner, Func>(f,owner,name,descr);
        _epList.push_back(ep);
        return ep;
    }

    inline const Smp::EntryPointCollection* GetEntryPoints() const override {
        return &_epList;
    }

    inline Smp::IEntryPoint* GetEntryPoint(Smp::String8 name) const override {
        return _epList.at(name);
    }

protected:
    EntryPointPublisher();

private:
    OwnedCollection<Smp::IEntryPoint> _epList;
};

}  // namespace simdeck
#endif  // __simdeck_EntryPointPublisher_HPP__

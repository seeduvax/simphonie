/*
 * @file IEntryPointPublisher.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_smpdk_IEntryPointPublisher_HPP__
#define __simph_smpdk_IEntryPointPublisher_HPP__

#include "Smp/IEntryPointPublisher.h"
#include "simph/smpdk/EntryPoint.hpp"
#include "simph/smpdk/Collection.hpp"

namespace simph {
namespace smpdk {
using namespace simph::smpdk;

class IEntryPointPublisher : virtual public Smp::IEntryPointPublisher {
public:
    IEntryPointPublisher();

    virtual ~IEntryPointPublisher();

    template <typename Owner, typename Func>
    inline void addEP(Smp::String8 name, Smp::String8 descr , Owner owner, Func f) {
        _epList.push_back(new TEntryPoint(f,owner,name,descr));
    }

    inline const Smp::EntryPointCollection* GetEntryPoints() const override {
        return &_epList;
    }

    inline Smp::IEntryPoint* GetEntryPoint(Smp::String8 name) const override {
        return _epList.at(name);
    }

private:
    OwnedCollection<Smp::IEntryPoint> _epList;
};

}  // namespace smpdk
}  // namespace simph
#endif  // __simph_smpdk_IEntryPointPublisher_HPP__

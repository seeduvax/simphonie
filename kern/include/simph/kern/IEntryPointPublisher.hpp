/*
 * @file IEntryPointPublisher.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_IEntryPointPublisher_HPP__
#define __simph_kern_IEntryPointPublisher_HPP__

#include "Smp/IEntryPointPublisher.h"
#include "simph/kern/Collection.hpp"
#include "simph/kern/EntryPoint.hpp"
#include "simph/sys/Runnable.hpp"

namespace simph {
    namespace kern {

using simph::sys::Runnable;

class IEntryPointPublisher: virtual public Smp::IEntryPointPublisher {
public:
    IEntryPointPublisher();

    virtual ~IEntryPointPublisher();

    inline void addEP( std::unique_ptr<Runnable> runnable,
                       Smp::String8 name,
                       Smp::String8 descr="") {
        _epList.push_back(
                new EntryPoint(std::move(runnable), name, descr, this)
        );
    }

    inline const Smp::EntryPointCollection* GetEntryPoints() const override {
        return &_epList;
    }

    inline Smp::IEntryPoint* GetEntryPoint(Smp::String8 name) const override {
        return _epList.at(name);
    }

private:
    CollectionOwner<Smp::IEntryPoint> _epList;
};

}} // namespace simph::kern
#endif // __simph_kern_IEntryPointPublisher_HPP__

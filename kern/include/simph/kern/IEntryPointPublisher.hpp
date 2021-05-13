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
#include "simph/kern/EntryPoint.hpp"
#include "simph/smpdk/Collection.hpp"
#include "simph/sys/Callback.hpp"
#include "simph/sys/Runnable.hpp"

namespace simph {
namespace kern {
using namespace simph::smpdk;

using simph::sys::Runnable;

class IEntryPointPublisher : virtual public Smp::IEntryPointPublisher {
public:
    IEntryPointPublisher();

    virtual ~IEntryPointPublisher();

    inline void addEP(std::unique_ptr<Runnable> runnable, Smp::String8 name, Smp::String8 descr = "") {
        _epList.push_back(new EntryPoint(std::move(runnable), name, descr, this));
    }

    template <typename _Callable, typename... _Args>
    inline void addEP(Smp::String8 name, Smp::String8 descr, _Callable&& func, _Args&&... args) {
        return addEP(simph::sys::Callback::create(std::forward<_Callable>(func), std::forward<_Args>(args)...), name,
                     descr);
    };

    inline const Smp::EntryPointCollection* GetEntryPoints() const override {
        return &_epList;
    }

    inline Smp::IEntryPoint* GetEntryPoint(Smp::String8 name) const override {
        return _epList.at(name);
    }

private:
    CollectionOwner<Smp::IEntryPoint> _epList;
};

}  // namespace kern
}  // namespace simph
#endif  // __simph_kern_IEntryPointPublisher_HPP__

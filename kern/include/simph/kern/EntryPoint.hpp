/*
 * @file EntryPoint.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_EntryPoint_HPP__
#define __simph_kern_EntryPoint_HPP__

#include "Smp/IEntryPoint.h"
#include "simph/smpdk/Object.hpp"
#include "simph/sys/Runnable.hpp"
#include "simph/sys/Callback.hpp"
#include <memory>

namespace simph {
namespace kern {
using namespace simph::smpdk;
using simph::sys::Runnable;

class EntryPoint : public Object, virtual public Smp::IEntryPoint {
public:
    EntryPoint(std::unique_ptr<Runnable> runnable, Smp::String8 name, Smp::String8 descr = "",
               Smp::IObject* parent = nullptr);

    virtual ~EntryPoint();

    inline void Execute() const override {
        _runnable->run();
    }
    template <typename _Callable, typename... _Args>  
    static Smp::IEntryPoint* Create(
                                Smp::String8 name,
                                Smp::String8 descr,
                                Smp::IObject* parent,
                                _Callable&& func,
                                _Args&&... args) {
        return new EntryPoint(
                simph::sys::Callback::create(std::forward<_Callable>(func),std::forward<_Args>(args)...),
                name,descr,parent);
    }

private:
    std::unique_ptr<Runnable> _runnable;
};

}  // namespace kern
}  // namespace simph
#endif  // __simph_kern_EntryPoint_HPP__

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

#include "simph/kern/Object.hpp"
#include "simph/sys/Runnable.hpp"
#include "Smp/IEntryPoint.h"

namespace simph {
    namespace kern {

using simph::sys::Runnable;

class EntryPoint: public Object,
                  virtual public Smp::IEntryPoint {
public:
    EntryPoint( std::unique_ptr<Runnable> runnable,
                Smp::String8 name,
                Smp::String8 descr="",
                Smp::IObject* parent=nullptr );

    virtual ~EntryPoint();

    inline void Execute() const override {
        _runnable->run();
    }

private:
    std::unique_ptr<Runnable> _runnable;
};

}} // namespace simph::kern
#endif // __simph_kern_EntryPoint_HPP__
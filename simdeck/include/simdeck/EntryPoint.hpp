/*
 * @file EntryPoint.hpp
 *
 * Copyright 2024 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_EntryPoint_HPP__
#define __simdeck_EntryPoint_HPP__

#include <functional>
#include <memory>
#include "simdeck/Component.hpp"
#include "Smp/IEntryPoint.h"

namespace simdeck {

class EntryPoint: public Component, virtual public Smp::IEntryPoint {
public:
    EntryPoint(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent):
            Component(name,descr,parent) {
    }
    virtual ~EntryPoint() {
    }
    
    void Execute() const override {}
    
    template <typename Owner, typename Func>
    static inline Smp::IEntryPoint* Create(Smp::String8 name, Smp::String8 descr,
                Owner owner, Func f);
};
/**
 *
 */
template <typename Owner, typename Func>
class TEntryPoint: public EntryPoint {
public:
    /**
     * Default constructor.
     */
    TEntryPoint(Func f, Owner owner, Smp::String8 name, Smp::String8 descr="")
            : EntryPoint(name,descr,owner),
              _owner(owner), _func(f) {
    }
    /**
     * Destructor.
     */
    virtual ~TEntryPoint() {
    }
    
    // Smp::IEntryPoint implementation
    void Execute() const override {
        (_owner->*_func)();
    }
private:
    Owner _owner;
    Func _func;
};
template <typename Owner, typename Func>
inline Smp::IEntryPoint* EntryPoint::Create(
                                Smp::String8 name, Smp::String8 descr,
                                Owner owner, Func f) {
    return new TEntryPoint<Owner, Func>(f,owner,name,descr);
}

} // namespace simdeck
#endif // __simdeck_EntryPoint_HPP__

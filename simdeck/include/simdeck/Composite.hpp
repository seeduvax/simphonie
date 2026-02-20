/*
 * @file Composite.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_Composite_HPP__
#define __simdeck_Composite_HPP__
#include <iostream>
#include "simdeck/Object.hpp"
#include "simdeck/Collection.hpp"
#include "Smp/IComposite.h"
namespace simdeck {

/**
 *
 */
class AComposite: public virtual Smp::IComposite {
public:
    /**
     * Destructor.
     */
    virtual ~AComposite();

    const Smp::ContainerCollection* GetContainers() const override;
    Smp::IContainer* GetContainer(Smp::String8 name) const override;

protected:
    AComposite();
    Smp::IContainer* addContainer(Smp::String8 name, Smp::String8 descr = "");

private:
    OwnedCollection<Smp::IContainer> _containers;
};

/**
 *
 */
class Composite: public Object, public virtual AComposite {
public:
    Composite(Smp::String8 name, Smp::String8 descr = "", Smp::IObject* parent = nullptr);
    /**
     * Destructor.
     */
    virtual ~Composite() = default;
};

}  // namespace simdeck
#endif  // __simdeck_Composite_HPP__

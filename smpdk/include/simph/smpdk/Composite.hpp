/*
 * @file Composite.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_smpdk_Composite_HPP__
#define __simph_smpdk_Composite_HPP__
#include <iostream>
#include "Smp/IComposite.h"
#include "simph/smpdk/Collection.hpp"
namespace simph {
namespace smpdk {

/**
 *
 */
class Composite : public Object, public virtual Smp::IComposite {
public:
    /**
     * Default constructor.
     */
    Composite(Smp::String8 name, Smp::String8 descr = "", Smp::IObject* parent = nullptr);
    /**
     * Destructor.
     */
    virtual ~Composite();

    const Smp::ContainerCollection* GetContainers() const override;
    Smp::IContainer* GetContainer(Smp::String8 name) const override;

protected:
    void addContainer(Smp::String8 name, Smp::String8 descr = "");

private:
    CollectionOwner<Smp::IContainer> _containers;
};

}  // namespace smpdk
}  // namespace simph
#endif  // __simph_smpdk_Composite_HPP__

/*
 * @file Composite.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_Composite_HPP__
#define __simph_kern_Composite_HPP__
#include <iostream>
#include "Smp/IComposite.h"
#include "simph/kern/Collection.hpp"
namespace simph {
	namespace kern {

/**
 *
 */
class Composite: virtual public Object, public virtual Smp::IComposite {
public:
    /**
     * Default constructor.
     */
    Composite();
    /**
     * Destructor.
     */
    virtual ~Composite();

    const Smp::ContainerCollection* GetContainers() const;
    Smp::IContainer* GetContainer(Smp::String8 name) const;
    void dump(std::ostream& out,int level=0) const;
    void dump();
protected:
    void addContainer(Smp::String8 name);
private:
    Collection<Smp::IContainer> _containers;
};

}} // namespace simph::kern
#endif // __simph_kern_Composite_HPP__

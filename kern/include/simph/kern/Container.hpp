/*
 * @file Container.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_Container_HPP__
#define __simph_kern_Container_HPP__
#include "Smp/IContainer.h"
#include "simph/kern/Collection.hpp"

namespace simph {
	namespace kern {

/**
 *
 */
class Container: virtual public Object, virtual public Smp::IContainer {
public:
    /**
     * Default constructor.
     */
    Container();
    /**
     * Destructor.
     */
    virtual ~Container();
    // Smp::IComponent implementation
    const Smp::ComponentCollection* GetComponents() const;
    Smp::IComponent* GetComponent(Smp::String8 name) const;
    void AddComponent(Smp::IComponent* component);
    Smp::Int64 GetCount() const;
    Smp::Int64 GetUpper() const;
    Smp::Int64 GetLower() const;

protected:
    virtual bool checkComponentType(Smp::IComponent* comp);

private:
    Collection<Smp::IComponent> _content;
};

}} // namespace simph::kern
#endif // __simph_kern_Container_HPP__

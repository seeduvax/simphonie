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
    Container(Smp::String8 name,Smp::String8 descr="",
                Smp::IObject* parent=nullptr);
    /**
     * Destructor.
     */
    virtual ~Container();

    // Smp::IComponent implementation
    const Smp::ComponentCollection* GetComponents() const override;
    Smp::IComponent* GetComponent(Smp::String8 name) const override;
    void AddComponent(Smp::IComponent* component) override;
    void DeleteComponent(Smp::IComponent* component) override;
    Smp::Int64 GetCount() const override;
    Smp::Int64 GetUpper() const override;
    Smp::Int64 GetLower() const override;

protected:
    virtual bool checkComponentType(Smp::IComponent* comp);

private:
    Collection<Smp::IComponent> _content;
};

}} // namespace simph::kern
#endif // __simph_kern_Container_HPP__

/*
 * @file Component.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_Component_HPP__
#define __simph_kern_Component_HPP__
#include "simph/kern/Object.hpp"
#include "Smp/IComponet.h"

namespace simph {
	namespace kern {

/**
 *
 */
class Component: public Object, virtual public IComponent {
public:
    /**
     * Default constructor.
     */
    Component();
    /**
     * Destructor.
     */
    virtual ~Component();

private:

};

}} // namespace simph::kern
#endif // __simph_kern_Component_HPP__

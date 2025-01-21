/*
 * @file CompositeComponent.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_smpdk_CompositeComponent_HPP__
#define __simph_smpdk_CompositeComponent_HPP__

#include "simph/smpdk/Component.hpp"
#include "simph/smpdk/Composite.hpp"

namespace simph {
namespace smpdk {

/**
 *
 */
class CompositeComponent: public Component, virtual public Composite {
    typedef Component Parent;
public:
    /**
     * Default constructor.
     */
    CompositeComponent(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent);
    /**
     * Destructor.
     */
    virtual ~CompositeComponent();

private:

};

}} // namespace simph::smpdk
#endif // __simph_smpdk_CompositeComponent_HPP__

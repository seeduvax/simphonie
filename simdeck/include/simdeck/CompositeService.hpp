/*
 * @file CompositeService.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_CompositeService_HPP__
#define __simdeck_CompositeService_HPP__

#include "simdeck/Service.hpp"
#include "simdeck/Composite.hpp"

namespace simdeck {

/**
 *
 */
class CompositeService: public Service, virtual public AComposite {
    typedef Service Parent;
public:
    /**
     * Default constructor.
     */
    CompositeService(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent);
    /**
     * Destructor.
     */
    virtual ~CompositeService();

private:

};

} // namespace simdeck
#endif // __simdeck_CompositeService_HPP__

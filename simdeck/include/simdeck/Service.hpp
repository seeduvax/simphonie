/*
 * @file Service.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_Service_HPP__
#define __simdeck_Service_HPP__

#include "simdeck/Component.hpp"
#include "Smp/IService.h"

namespace simdeck {

/**
 *
 */
class Service: public Component, virtual public Smp::IService {
    typedef Component Parent;
public:
    /**
     * Default constructor.
     */
    Service(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent);
    /**
     * Destructor.
     */
    virtual ~Service();

private:

};

} // namespace simdeck
#endif // __simdeck_Service_HPP__

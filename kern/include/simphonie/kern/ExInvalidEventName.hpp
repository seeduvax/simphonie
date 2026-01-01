/*
 * @file ExInvalidEventName.hpp
 *
 * Copyright 2026 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_kern_ExInvalidEventName_HPP__
#define __simphonie_kern_ExInvalidEventName_HPP__
#include "Smp/Services/InvalidEventName.h"
#include "simdeck/Exception.hpp"


namespace simphonie {
namespace kern {

/**
 *
 */
class ExInvalidEventName: public simdeck::Exception, virtual public Smp::Services::InvalidEventName {
public:
    /**
     * Default constructor.
     */
    ExInvalidEventName(Smp::IObject* sender);
    /**
     * Destructor.
     */
    virtual ~ExInvalidEventName();

private:

};

}} // namespace simphonie::kern
#endif // __simphonie_kern_ExInvalidEventName_HPP__

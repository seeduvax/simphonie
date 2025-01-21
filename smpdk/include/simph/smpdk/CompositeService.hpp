/*
 * @file CompositeService.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_smpdk_CompositeService_HPP__
#define __simph_smpdk_CompositeService_HPP__

#include "simph/smpdk/Service.hpp"
#include "simph/smpdk/Composite.hpp"

namespace simph {
namespace smpdk {

/**
 *
 */
class CompositeService: public Service, virtual public Composite {
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

}} // namespace simph::smpdk
#endif // __simph_smpdk_CompositeService_HPP__

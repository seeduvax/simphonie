/*
 * @file Service.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_smpdk_Service_HPP__
#define __simph_smpdk_Service_HPP__

#include "simph/smpdk/Component.hpp"
#include "Smp/IService.h"

namespace simph {
namespace smpdk {

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

}} // namespace simph::smpdk
#endif // __simph_smpdk_Service_HPP__

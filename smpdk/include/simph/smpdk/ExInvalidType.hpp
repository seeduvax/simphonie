/*
 * @file ExInvalidType.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_smpdk_ExInvalidType_HPP__
#define __simph_smpdk_ExInvalidType_HPP__
#include "Smp/InvalidType.h"
#include "simph/smpdk/Exception.hpp"

namespace simph {
namespace smpdk {

/**
 *
 */
class ExInvalidType: public Exception, virtual public Smp::InvalidType {
public:
    /**
     * Default constructor.
     */
    ExInvalidType(const Smp::IObject* sender, Smp::String8 msg);
    /**
     * Destructor.
     */
    virtual ~ExInvalidType();

private:

};

}} // namespace simph::smpdk
#endif // __simph_smpdk_ExInvalidType_HPP__

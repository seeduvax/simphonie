/*
 * @file ExInvalidFieldType.hpp
 *
 * Copyright 2020 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_ExInvalidFieldType_HPP__
#define __simph_kern_ExInvalidFieldType_HPP__
#include "simph/smpdk/Exception.hpp"
#include "Smp/InvalidFieldType.h"

namespace simph {
	namespace kern {
using namespace simph::smpdk;

/**
 *
 */
class ExInvalidFieldType: public Exception,
            virtual public Smp::InvalidFieldType {
public:
    /**
     * Default constructor.
     */
    ExInvalidFieldType(Smp::IObject* sender, Smp::String8 descr);
    /**
     * Destructor.
     */
    virtual ~ExInvalidFieldType();

private:

};

}} // namespace simph::kern
#endif // __simph_kern_ExInvalidFieldType_HPP__

/*
 * @file ExInvalidFieldName.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_smpdk_ExInvalidFieldName_HPP__
#define __simph_smpdk_ExInvalidFieldName_HPP__
#include "Smp/InvalidFieldName.h"
#include "simph/smpdk/Exception.hpp"

namespace simph {
namespace smpdk {

/**
 *
 */
class ExInvalidFieldName: public Exception, virtual public Smp::InvalidFieldName {
    typedef Exception Parent;
public:
    /**
     * Default constructor.
     */
    ExInvalidFieldName(const Smp::IObject* sender, Smp::String8 invalidName);
    /**
     * Destructor.
     */
    virtual ~ExInvalidFieldName();
    // Smp::InvalidFieldName implementation
    Smp::String8 GetFieldName() const noexcept override;

private:
    std::string _invalidName;

};

}} // namespace simph::smpdk
#endif // __simph_smpdk_ExInvalidFieldName_HPP__

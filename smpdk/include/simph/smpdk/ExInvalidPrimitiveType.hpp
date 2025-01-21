/*
 * @file ExInvalidPrimitiveType.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_smpdk_ExInvalidPrimitiveType_HPP__
#define __simph_smpdk_ExInvalidPrimitiveType_HPP__
#include "Smp/Publication/InvalidPrimitiveType.h"
#include "simph/smpdk/Exception.hpp"

namespace simph {
namespace smpdk {
using namespace simph::smpdk;

/**
 *
 */
class ExInvalidPrimitiveType : public Exception, virtual public Smp::Publication::InvalidPrimitiveType {
public:
    /**
     * Default constructor.
     */
    ExInvalidPrimitiveType(Smp::IObject* sender, Smp::PrimitiveTypeKind type);
    /**
     * Destructor.
     */
    virtual ~ExInvalidPrimitiveType();
    // Smp::Publication::InvalidPrimitiveType implementation
    Smp::String8 GetTypeName() const noexcept override;
    Smp::PrimitiveTypeKind GetType() const noexcept override;

private:
    Smp::PrimitiveTypeKind _type;
};

}  // namespace smpdk
}  // namespace simph
#endif  // __simph_smpdk_ExInvalidPrimitiveType_HPP__

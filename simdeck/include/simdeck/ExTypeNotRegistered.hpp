/*
 * @file ExTypeNotRegistered.hpp
 *
 * Copyright 2020 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_ExTypeNotRegistered_HPP__
#define __simdeck_ExTypeNotRegistered_HPP__

#include "Smp/Publication/TypeNotRegistered.h"
#include "simdeck/Exception.hpp"

namespace simdeck {

/**
 *
 */
class ExTypeNotRegistered : public simdeck::Exception, virtual public Smp::Publication::TypeNotRegistered {
public:
    /**
     * Default constructor.
     */
    ExTypeNotRegistered(const Smp::IObject* sender, Smp::Uuid typeUuid);
    /**
     * Destructor.
     */
    virtual ~ExTypeNotRegistered();
    // Smp::Publication::TypeNotRegistered
    Smp::Uuid GetUuid() const noexcept override;

private:
    Smp::Uuid _uuid;
};

}  // namespace simdeck
#endif  // __simdeck_ExTypeNotRegistered_HPP__

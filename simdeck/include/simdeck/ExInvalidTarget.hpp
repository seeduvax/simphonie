/*
 * @file ExInvalidTarget.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_ExInvalidTarget_HPP__
#define __simdeck_ExInvalidTarget_HPP__
#include "simdeck/Exception.hpp"
#include "Smp/InvalidTarget.h"

namespace simdeck {
using namespace simdeck;

/**
 *
 */
class ExInvalidTarget : public Exception, virtual public Smp::InvalidTarget {
public:
    /**
     * Default constructor.
     */
    ExInvalidTarget(Smp::IOutputField* source, Smp::IField* target);
    /**
     * Destructor.
     */
    virtual ~ExInvalidTarget();
    // Smp::InvalidTarget implmentation
    Smp::IOutputField* GetSource() const noexcept override;
    Smp::IField* GetTarget() const noexcept override;

private:
    Smp::IOutputField* _source;
    Smp::IField* _target;
};

}  // namespace simdeck
#endif  // __simdeck_ExInvalidTarget_HPP__

/*
 * @file ExInvalidTarget.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_ExInvalidTarget_HPP__
#define __simph_kern_ExInvalidTarget_HPP__
#include "Smp/InvalidTarget.h"
#include "simph/smpdk/Exception.hpp"

namespace simph {
namespace kern {
using namespace simph::smpdk;

/**
 *
 */
class ExInvalidTarget : public Exception, virtual public Smp::InvalidTarget {
public:
    /**
     * Default constructor.
     */
    ExInvalidTarget(Smp::IDataflowField* source, Smp::IField* target);
    /**
     * Destructor.
     */
    virtual ~ExInvalidTarget();
    // Smp::InvalidTarget implmentation
    Smp::IDataflowField* GetSource() const noexcept override;
    Smp::IField* GetTarget() const noexcept override;

private:
    Smp::IDataflowField* _source;
    Smp::IField* _target;
};

}  // namespace kern
}  // namespace simph
#endif  // __simph_kern_ExInvalidTarget_HPP__

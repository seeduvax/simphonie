/*
 * @file ExInvalidTarget.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_smpdk_ExInvalidTarget_HPP__
#define __simph_smpdk_ExInvalidTarget_HPP__
#include "Smp/InvalidTarget.h"
#include "simph/smpdk/Exception.hpp"

namespace simph {
namespace smpdk {
using namespace simph::smpdk;

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

}  // namespace smpdk
}  // namespace simph
#endif  // __simph_smpdk_ExInvalidTarget_HPP__

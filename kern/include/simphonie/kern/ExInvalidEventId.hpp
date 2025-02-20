/*
 * @file ExInvalidEventId.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_kern_ExInvalidEventId_HPP__
#define __simphonie_kern_ExInvalidEventId_HPP__
#include "Smp/Services/InvalidEventId.h"
#include "simdeck/Exception.hpp"

namespace simphonie {
namespace kern {
using namespace simdeck;
/**
 *
 */
class ExInvalidEventId : public Exception, virtual public Smp::Services::InvalidEventId {
public:
    /**
     * Default constructor.
     */
    ExInvalidEventId(Smp::IObject* sender, Smp::Services::EventId invalidId);
    /**
     * Destructor.
     */
    virtual ~ExInvalidEventId();
    // Smp::Services::InvalidEventId implementation
    Smp::Services::EventId GetInvalidEventId() const noexcept override;

private:
    Smp::Services::EventId _invalidId;
};

}  // namespace kern
}  // namespace simph
#endif  // __simphonie_kern_ExInvalidEventId_HPP__

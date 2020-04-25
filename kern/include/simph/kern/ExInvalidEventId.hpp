/*
 * @file ExInvalidEventId.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_ExInvalidEventId_HPP__
#define __simph_kern_ExInvalidEventId_HPP__
#include "simph/smpdk/Exception.hpp"
#include "Smp/Services/InvalidEventId.h"

namespace simph {
	namespace kern {
using namespace simph::smpdk;
/**
 *
 */
class ExInvalidEventId: public Exception,
                    virtual public Smp::Services::InvalidEventId {
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

}} // namespace simph::kern
#endif // __simph_kern_ExInvalidEventId_HPP__

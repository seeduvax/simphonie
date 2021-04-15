/*
 * @file ExTypeNotRegistered.hpp
 *
 * Copyright 2020 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_ExTypeNotRegistered_HPP__
#define __simph_kern_ExTypeNotRegistered_HPP__

#include "Smp/Publication/TypeNotRegistered.h"
#include "simph/smpdk/Exception.hpp"

namespace simph {
namespace kern {

/**
 *
 */
class ExTypeNotRegistered : public simph::smpdk::Exception, virtual public Smp::Publication::TypeNotRegistered {
public:
    /**
     * Default constructor.
     */
    ExTypeNotRegistered(Smp::IObject* sender, Smp::Uuid typeUuid);
    /**
     * Destructor.
     */
    virtual ~ExTypeNotRegistered();
    // Smp::Publication::TypeNotRegistered
    Smp::Uuid GetUuid() const noexcept override;

private:
    Smp::Uuid _uuid;
};

}  // namespace kern
}  // namespace simph
#endif  // __simph_kern_ExTypeNotRegistered_HPP__

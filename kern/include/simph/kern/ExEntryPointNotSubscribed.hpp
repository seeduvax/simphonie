/*
 * @file ExEntryPointNotSubscribed.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_ExEntryPointNotSubscribed_HPP__
#define __simph_kern_ExEntryPointNotSubscribed_HPP__
#include "Smp/Services/EntryPointNotSubscribed.h"
#include "simph/smpdk/Exception.hpp"

namespace simph {
namespace kern {
using namespace simph::smpdk;
/**
 *
 */
class ExEntryPointNotSubscribed : public Exception, virtual public Smp::Services::EntryPointNotSubscribed {
public:
    /**
     * Default constructor.
     */
    ExEntryPointNotSubscribed(Smp::IObject* sender, const Smp::IEntryPoint* ep, Smp::String8 evName);
    /**
     * Destructor.
     */
    virtual ~ExEntryPointNotSubscribed();
    // Smp::Services::EntryPointNotSubscribed implementation
    const Smp::IEntryPoint* GetEntryPoint() const noexcept override;
    Smp::String8 GetEventName() const noexcept override;

private:
    const Smp::IEntryPoint* _entryPoint;
    Smp::String8 _evName;
};

}  // namespace kern
}  // namespace simph
#endif  // __simph_kern_ExEntryPointNotSubscribed_HPP__

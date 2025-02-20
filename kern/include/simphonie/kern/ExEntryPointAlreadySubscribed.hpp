/*
 * @file ExEntryPointAlreadySubscribed.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_kern_ExEntryPointAlreadySubscribed_HPP__
#define __simphonie_kern_ExEntryPointAlreadySubscribed_HPP__
#include "Smp/Services/EntryPointAlreadySubscribed.h"
#include "simdeck/Exception.hpp"

namespace simphonie {
namespace kern {
using namespace simdeck;
/**
 *
 */
class ExEntryPointAlreadySubscribed : public Exception, virtual public Smp::Services::EntryPointAlreadySubscribed {
public:
    /**
     * Default constructor.
     */
    ExEntryPointAlreadySubscribed(Smp::IObject* sender, const Smp::IEntryPoint* ep, Smp::String8 evName);
    /**
     * Destructor.
     */
    virtual ~ExEntryPointAlreadySubscribed();
    // Smp::Services::EntryPointAlreadySubscribed implementation
    const Smp::IEntryPoint* GetEntryPoint() const noexcept override;
    Smp::String8 GetEventName() const noexcept override;

private:
    const Smp::IEntryPoint* _entryPoint;
    Smp::String8 _evName;
};

}  // namespace kern
}  // namespace simph
#endif  // __simphonie_kern_ExEntryPointAlreadySubscribed_HPP__

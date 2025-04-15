/*
 * @file LoggerOStream.hpp
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_kern_LoggerOStream_HPP__
#define __simphonie_kern_LoggerOStream_HPP__

#include "Smp/IObject.h"
#include "Smp/String8.h"
#include "simdeck/Component.hpp"
#include "simphonie/kern/ILoggerBackend.hpp"
#include "simphonie/kern/LoggerEvent.hpp"

namespace simphonie {
namespace kern {
using namespace simdeck;

class LoggerOStream : public Component, virtual public ILoggerBackend {
public:
    LoggerOStream(Smp::String8 name, Smp::String8 descr = "", Smp::IObject* parent = nullptr);

    void log(const LoggerEvent& event) override;
};

} /* namespace kern */
} /* namespace simphonie */

#endif /* __simphonie_kern_LoggerOStream_HPP__ */

/*
 * @file LoggerOstream.hpp
 *
 * Copyright 2025 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_kern_LoggerOstream_HPP__
#define __simphonie_kern_LoggerOstream_HPP__

#include "Smp/Bool.h"
#include "Smp/IObject.h"
#include "Smp/IPublication.h"
#include "Smp/String8.h"
#include "simdeck/Component.hpp"
#include "simphonie/kern/ILoggerBackend.hpp"
#include "simphonie/kern/LoggerEvent.hpp"

namespace simphonie {
namespace kern {
using namespace simdeck;

class LoggerOstream : public Component, virtual public ILoggerBackend {
public:
    LoggerOstream(Smp::String8 name, Smp::String8 descr = "", Smp::IObject* parent = nullptr);

    void log(LoggerEvent event) override;
};

} /* namespace kern */
}  // namespace simphonie
#endif /* __simphonie_kern_LoggerOstream_HPP__ */

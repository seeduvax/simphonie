/*
 * @file FMILoggerBackend.hpp
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_fmi_FMILoggerBackend_HPP__
#define __simphonie_fmi_FMILoggerBackend_HPP__

#include "Smp/IObject.h"
#include "Smp/String8.h"
#include "simdeck/Component.hpp"
#include "simphonie/kern/ILoggerBackend.hpp"
#include "simphonie/kern/LoggerEvent.hpp"

namespace simphonie {
namespace fmi {

class FMILoggerBackend : public simdeck::Component, virtual public simphonie::kern::ILoggerBackend {
public:
    FMILoggerBackend(cppfmu::Logger logger, Smp::String8 name, Smp::String8 descr = "", Smp::IObject* parent = nullptr);
    virtual ~FMILoggerBackend() = default;

    void log(const simphonie::kern::LoggerEvent& event) override;

private:
/* TODO find what fmi standard use to refer to the logger.
    cppfmu::Logger _logger;
 */
};

} /* namespace fmi */
} /* namespace simphonie */
#endif /* __simphonie_fmi_FMILoggerBackend_HPP__ */

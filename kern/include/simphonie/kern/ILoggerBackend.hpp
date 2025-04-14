/*
 * @file ILoggerBackend.hpp
 *
 * Copyright 2025 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_kern_ILoggerBackend_HPP__
#define __simphonie_kern_ILoggerBackend_HPP__

#include "simphonie/kern/LoggerEvent.hpp"

namespace simphonie {
namespace kern {

class ILoggerBackend {
public:
    virtual void log(simphonie::kern::LoggerEvent event) = 0;
};

} /* namespace kern */
}  // namespace simphonie
#endif /* __simphonie_kern_ILoggerBackend_HPP__ */

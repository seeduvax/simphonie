/*
 * @file LoggerFile.hpp
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_kern_LoggerFile_HPP__
#define __simphonie_kern_LoggerFile_HPP__

#include <fstream>
#include "Smp/IObject.h"
#include "Smp/IPublication.h"
#include "Smp/String8.h"
#include "simdeck/Component.hpp"
#include "simphonie/kern/ILoggerBackend.hpp"
#include "simphonie/kern/LoggerEvent.hpp"

namespace simphonie {
namespace kern {
using namespace simdeck;

class LoggerFile : public Component, virtual public ILoggerBackend {
public:
    LoggerFile(Smp::String8 name, Smp::String8 descr = "", Smp::IObject* parent = nullptr);
    ~LoggerFile() override;

    void log(const LoggerEvent& event) override;

protected:
    void publish(Smp::IPublication* receiver);
    void configure() override;

private:
    std::string _filepath;
    std::ofstream _file;
};

} /* namespace kern */
} /* namespace simphonie */

#endif /* __simphonie_kern_LoggerFile_HPP__ */

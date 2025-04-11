/*
 * @file Logger.hpp
 *
 * Copyright 2025 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_kern_Logger_HPP__
#define __simphonie_kern_Logger_HPP__
#include <mutex>
#include <queue>
#include <thread>
#include "Smp/Services/ILogger.h"
#include "simdeck/Component.hpp"
#include "simdeck/Container.hpp"
#include "simphonie/kern/LoggerEvent.hpp"
#include "simphonie/kern/Simulator.hpp"
#include "simphonie/sys/Logger.hpp"

/*
 * simph.log.1: TOFINISH
 * simph.log.cfg.1: error (not defined in Smp::Ipublication::IPublishedField => Create it or use void* one?)
 * simph.log.cfg.2: misunderstand ("the host simulator")
 * simph.log.cfg.3: done
 * simph.log.cfg.4: error (not defined in Smp::Ipublication::IPublishedField => Create it or use void* one?)
 *                  + misunderstand ("to network send", what to define?)
 * simph.log.cfg.5: done
 * simph.log.ev.1: TODO
 * simph.log.ev.2: TODO
 * simph.log.ev.3: TODO
 * simph.log.ev.4: TODO
 * simph.log.ev.5: TODO
 * simph.log.ev.6: TODO
 * simph.log.unsync.1: TODO
 * simph.log.net.1: TODO
 * simph.log.net.2: TODO
 */

namespace simphonie {
namespace kern {
using namespace simdeck;

class Logger : public Component, virtual public Smp::Services::ILogger, virtual public AComposite {
public:
    Logger(Smp::String8 name, Smp::String8 descr = "", Smp::IObject* parent = nullptr);

    Smp::Services::LogMessageKind QueryLogMessageKind(Smp::String8 messageKindName) override;
    void Log(const Smp::IObject* sender, Smp::String8 message, Smp::Services::LogMessageKind kind = 0) override;

protected:
    void publish(Smp::IPublication* receiver) override;
    static std::string buildLogString(LoggerEvent event);

private:
    Smp::ISimulator* _simulator;

    friend
};

} /* namespace kern */
}  // namespace simphonie
#endif /* __simphonie_kern_Logger_HPP__ */

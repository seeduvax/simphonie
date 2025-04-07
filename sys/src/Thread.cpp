/*
 * @file Thread.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/sys/Thread.hpp"

namespace simphonie {
namespace sys {

// --------------------------------------------------------------------
// ..........................................................
void threadEntryPoint(Thread& th) {
    th.run();
}
// --------------------------------------------------------------------
// ..........................................................
Thread::Thread(std::string name, Runnable* toRun) : _toRun(toRun), _name(name) {}
// ..........................................................
Thread::~Thread() {
    if (_th != nullptr) {
        join();
        _th.reset();
    }
}
// --------------------------------------------------------------------
// ..........................................................
void Thread::start() {
    if (_th != nullptr) {
        join();
    }
    std::thread tmp(threadEntryPoint, std::ref(*this));
    _th.reset(new std::thread(threadEntryPoint, std::ref(*this)));
}

}  // namespace sys
}  // namespace simph

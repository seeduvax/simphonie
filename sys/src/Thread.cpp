/*
 * @file Thread.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/sys/Thread.hpp"

namespace simph {
	namespace sys {

// --------------------------------------------------------------------
// ..........................................................
void threadEntryPoint(Thread* th) {
    th->run();
}
// --------------------------------------------------------------------
// ..........................................................
Thread::Thread(std::string name, Runnable* toRun):
        _toRun(toRun),
        _name(name),
        _running(false),
        _th(nullptr) {
}
// ..........................................................
Thread::~Thread() {
    if (_running) {
        join();
    }
    if (_th!=nullptr) {
        delete _th;
    }
}
// --------------------------------------------------------------------
// ..........................................................
void Thread::start() {
    if (_running) {
        join();
        delete _th;
    }
    _th=new std::thread(threadEntryPoint,this);
}

}} // namespace simph::sys

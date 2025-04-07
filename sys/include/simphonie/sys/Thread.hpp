/*
 * @file Thread.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_sys_Thread_HPP__
#define __simphonie_sys_Thread_HPP__
#include <string>
#include <thread>
#include "simphonie/sys/Runnable.hpp"
namespace simphonie {
namespace sys {

/**
 *
 */
class Thread {
public:
    /**
     * Default constructor.
     */
    Thread(std::string name, Runnable* toRun);
    /**
     * Destructor.
     */
    virtual ~Thread();

    void start();

    inline void run() {
        _toRun->run();
    }

    inline void join() {
        if (_th->joinable()) {
            _th->join();
        }
    }

    inline std::string getName() {
        return _name;
    }

    inline bool isCurrentThread() {
        return std::this_thread::get_id() == _th->get_id();
    }

    inline static std::thread::id GetCurrentThreadId() {
        return std::this_thread::get_id();
    }

private:
    Runnable* _toRun = nullptr;
    std::string _name;
    std::unique_ptr<std::thread> _th;
};

}  // namespace sys
}  // namespace simph
#endif  // __simphonie_sys_Thread_HPP__

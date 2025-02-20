/*
 * @file Synchro.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_sys_Synchro_HPP__
#define __simphonie_sys_Synchro_HPP__
#include <condition_variable>
#include <mutex>

#define Synchronized(mutex_var) std::unique_lock<std::mutex> _synchronized_lock(mutex_var);
#define MonitorWaitUntil(cond_var, timeout) cond_var.wait_until(_synchronized_lock, timeout);
#define MonitorWait(cond_var) cond_var.wait(_synchronized_lock);
#define MonitorNotify(cond_var) cond_var.notify();
#define MonitorNotifyAll(cond_var) cond_var.notify_all();

#endif  // __simphonie_sys_Synchro_HPP__

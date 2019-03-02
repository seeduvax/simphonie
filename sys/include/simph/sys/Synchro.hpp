/*
 * @file Synchro.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_sys_Synchro_HPP__
#define __simph_sys_Synchro_HPP__
#include <mutex>

#define Synchronized(mutex_var) \
    std::lock_guard<std::mutex> _synchronized_lock(mutex_var);

#endif // __simph_sys_Synchro_HPP__

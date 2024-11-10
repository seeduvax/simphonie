/*
 * @file DLib.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/sys/DLib.hpp"

#if defined(WIN32) || defined(_WIN32) || defined(__NT__) || defined(_WIN64)
#include <Windows.h>
#define OS_IS_WINDOWS
#else
#include <dlfcn.h>
#endif

namespace simph {
namespace sys {
// --------------------------------------------------------------------
// ..........................................................
DLib::DLib(const char* libName) : _name(libName) {
#ifdef OS_IS_WINDOWS
    _lib = reinterpret_cast<void*>(LoadLibrary(libName));
#else
    _lib = dlopen(libName, RTLD_NOW | RTLD_LAZY);
#endif
    if (_lib == nullptr) {
        std::ostringstream msg;
#ifdef OS_IS_WINDOWS
        msg << "Can't load library " << libName;
#else
        msg << "Can't load library " << libName << ": " << dlerror();
#endif
        LOGE(msg.str());
        throw std::runtime_error(msg.str());
    }
}
// ..........................................................
DLib::~DLib() {
#ifdef OS_IS_WINDOWS
    FreeLibrary(reinterpret_cast<HMODULE>(_lib));
#else
    dlclose(_lib);
#endif
}
// --------------------------------------------------------------------
// ..........................................................
void* DLib::getEntryHandle(const char* symbol) {
#ifdef OS_IS_WINDOWS
        return reinterpret_cast<void*>(GetProcAddress(reinterpret_cast<HMODULE>(_lib), symbol));
#else
        void* fptr = dlsym(_lib, symbol);
        if (fptr == nullptr) {
            LOGW("Can't find Symbol " << symbol << " from lib " << _name << ": " << dlerror());
        }
        return fptr;
#endif
}

}  // namespace sys
}  // namespace simph

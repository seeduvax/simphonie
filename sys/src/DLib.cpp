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

namespace simph {
namespace sys {
// --------------------------------------------------------------------
// ..........................................................
DLib::DLib(const char* libName) : _name(libName) {
#ifdef WIN32
    _lib = LoadLibrary(libName);
#else
    _lib = dlopen(libName, RTLD_NOW | RTLD_LAZY);
    if (_lib == nullptr) {
        std::ostringstream msg;
        msg << "Can't load library " << libName << ": " << dlerror();
        LOGE(msg.str());
        throw std::runtime_error(msg.str());
    }
#endif
}
// ..........................................................
DLib::~DLib() {
#ifdef WIN32
    FreeLibrary(_lib);
#else
    dlclose(_lib);
#endif
}

}  // namespace sys
}  // namespace simph

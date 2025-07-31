/*
 * @file DLib.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/sys/DLib.hpp"
#include "simphonie/sys/Logger.hpp"

#if defined(WIN32) || defined(_WIN32) || defined(__NT__) || defined(_WIN64)
#include <windows.h>
#define OS_IS_WINDOWS
#else
#include <dlfcn.h>
#include <unistd.h>
#endif

namespace simphonie {
namespace sys {

#ifdef OS_IS_WINDOWS
class NativeLib: public DLib::IHandler {
public:
    NativeLib(const char* libName): _name(libName) {
        _lib = LoadLibraryA(libName);
        if (_lib == NULL) {
            auto errCode=GetLastError();
            std::ostringstream msg;
            msg << "Can't load library " << libName << "[ errCode = " 
                << errCode << " ]" ;
            LOGE(msg.str());
            throw std::runtime_error(msg.str());
        }
    }
    virtual ~NativeLib() {
        FreeLibrary(_lib);
    }
    virtual void* getEntry(const char* symbol) const override {
        auto fptr=GetProcAddress(reinterpret_cast<HMODULE>(_lib), symbol);
        if (fptr == NULL) {
            auto errCode=GetLastError();
            LOGW("Can't find symbol " << symbol << " from lib " << _name
                << "[ errCode = " << errCode << " ]");
            return nullptr;
        }
        return reinterpret_cast<void*>(fptr);
    }
    virtual const std::string& getName() const override {
        return _name;
    }
private:
    HMODULE _lib;
    std::string _name;
};
#else
class NativeLib: public DLib::IHandler {
public:
    NativeLib(const char* libName, bool global) : _name(libName) {
        std::string err = "";
        int flg;
        if (global) {
            flg = RTLD_GLOBAL;
        }
        else {
            flg = RTLD_LOCAL;
        }
        _lib = dlopen(libName, RTLD_NOW | RTLD_LAZY | flg);
        if (_lib==nullptr) {
            std::ostringstream oss;
            oss << "Can't load library " << libName << ": " << std::endl
                << "- tried " << libName << ": " << dlerror() << std::endl;
            std::string libFile=libName;
            libFile+=".so";
            _lib = dlopen(libFile.c_str(), RTLD_NOW | RTLD_LAZY | flg);
            if (_lib==nullptr) {
                oss << "- tried " << libFile << ": " << dlerror() << std::endl;
                libFile="lib"+libFile;
                _lib = dlopen(libFile.c_str(), RTLD_NOW | RTLD_LAZY | flg);
                if (_lib == nullptr) {
                    oss << "- tried " << libFile << ": " << dlerror();
                    LOGE(oss.str());
                    throw std::runtime_error(oss.str());
                }
            }
        }
    }
    virtual ~NativeLib() {
        dlclose(_lib);
    }
    void* getEntry(const char* symbol) const override {
        void* fptr = dlsym(_lib, symbol);
        if (fptr == nullptr) {
            LOGW("Can't find Symbol " << symbol << " from lib " << _name << ": " << dlerror());
        }
        return fptr;
    }
    const std::string& getName() const override {
        return _name;
    }
private:
    void* _lib;
    std::string _name;
};
#endif
// --------------------------------------------------------------------
// ..........................................................
DLib::DLib(const char* libName, bool global) : _libH(new NativeLib(libName, global)) {}
// ..........................................................
DLib::~DLib() {
}

}  // namespace sys
}  // namespace simph

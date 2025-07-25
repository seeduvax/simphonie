/*
 * @file DLib.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_sys_DLib_HPP__
#define __simphonie_sys_DLib_HPP__
#include <string>
#include <memory>

namespace simphonie {
namespace sys {

class NativeLib;
/**
 *
 */
class DLib {
public:
    /**
     * Default constructor.
     */
    DLib(const char* libName, bool global = true);
    /**
     * Destructor.
     */
    virtual ~DLib();

    template <typename FT>
    FT getEntry(const char* symbol) {
        return reinterpret_cast<FT>(_libH->getEntry(symbol));
    }
    inline const std::string& getName() const {
        return _libH->getName();
    }

    class IHandler {
    public:
        virtual ~IHandler() = default;
        virtual const std::string& getName() const =0;
        virtual void* getEntry(const char* symbol) const=0;
    };

private:
    std::unique_ptr<IHandler> _libH;
};

}  // namespace sys
}  // namespace simph
#endif  // __simphonie_sys_DLib_HPP__

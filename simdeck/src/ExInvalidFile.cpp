/*
 * @file ExInvalidFile.cpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simdeck/ExInvalidFile.hpp"
#include <sstream>

namespace simdeck {
// --------------------------------------------------------------------
// ..........................................................
ExInvalidFile::ExInvalidFile(const Smp::IObject* sender,
                            Smp::String8 fileName, Smp::String8 errorMsg):
                _fileName(fileName), _errorMsg(errorMsg) {
    setName("InvalidFile");
    setSender(sender);
    std::ostringstream d;
    d << "invalid file " << _fileName;
    if ( std::string(errorMsg) != "" ) {
        d << ": " << errorMsg;
    }
    setDescription(d.str().c_str());
    setMessage();
}
// ..........................................................
ExInvalidFile::~ExInvalidFile() {
}
// --------------------------------------------------------------------
// ..........................................................
Smp::String8 ExInvalidFile::GetFileName() const noexcept {
    return _fileName.c_str();
}
// ..........................................................
Smp::String8 ExInvalidFile::GetErrorMessage() const noexcept {
    return _errorMsg.c_str();
}

} // namespace simdeck

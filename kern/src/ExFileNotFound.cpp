/*
 * @file FileNotFound.cpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/kern/ExFileNotFound.hpp"
#include "simphonie/sys/DLib.hpp"
#include <sstream>

namespace simphonie {
namespace kern {
// --------------------------------------------------------------------
// ..........................................................
ExFileNotFound::ExFileNotFound(const Smp::IObject* sender, Smp::String8 fileName) : _fileName(fileName) {
    setName("FileNotFound");
    setSender(sender);
    std::ostringstream d;
    d << "File " << fileName << " wasn't found";
    setDescription(d.str().c_str());
    setMessage();
}
// ..........................................................
ExFileNotFound::~ExFileNotFound() {}
// ..........................................................
Smp::String8 ExFileNotFound::GetFileName() const noexcept {
    return _fileName;
}

}  // namespace kern
}  // namespace simph

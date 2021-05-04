/*
 * @file LibraryNotFound.cpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/ExLibraryNotFound.hpp"
#include "simph/sys/DLib.hpp"

namespace simph {
namespace kern {
// --------------------------------------------------------------------
// ..........................................................
ExLibraryNotFound::ExLibraryNotFound(const Smp::IObject* sender, Smp::String8 libraryname) : _libraryName(libraryname) {
    setName("LibraryNotFound");
    setSender(sender);
    std::ostringstream d;
    d << "Library " << libraryname << " wasn't found";
    setDescription(d.str().c_str());
    setMessage();
}
// ..........................................................
ExLibraryNotFound::~ExLibraryNotFound() {}
// ..........................................................
Smp::String8 ExLibraryNotFound::GetLibraryName() const noexcept {
    return _libraryName;
}

}  // namespace kern
}  // namespace simph

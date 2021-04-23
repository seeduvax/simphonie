/*
 * @file LibraryNotFound.hpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_LibraryNotFound_HPP__
#define __simph_kern_LibraryNotFound_HPP__
#include "Smp/IObject.h"
#include "Smp/LibraryNotFound.h"
#include "simph/kern/Logger.hpp"
#include "simph/smpdk/Exception.hpp"

namespace simph {
namespace kern {

/**
 *
 */
class ExLibraryNotFound : public Exception, virtual public Smp::LibraryNotFound {
public:
    /**
     * Default constructor.
     */
    ExLibraryNotFound(const Smp::IObject* sender, Smp::String8 libraryname);
    /**
     * Destructor.
     */
    virtual ~ExLibraryNotFound();

    Smp::String8 GetLibraryName() const noexcept override;

private:
    Smp::String8 _libraryName;
};

}  // namespace kern
}  // namespace simph
#endif  // __simph_kern_LibraryNotFound_HPP__

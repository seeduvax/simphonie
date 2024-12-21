/*
 * @file FileNotFound.hpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_FileNotFound_HPP__
#define __simph_kern_FileNotFound_HPP__
#include "Smp/IObject.h"
#include "Smp/FileNotFound.h"
#include "simph/smpdk/Exception.hpp"

namespace simph {
namespace kern {

/**
 *
 */
class ExFileNotFound : public simph::smpdk::Exception, virtual public Smp::FileNotFound {
public:
    /**
     * Default constructor.
     */
    ExFileNotFound(const Smp::IObject* sender, Smp::String8 fileName);
    /**
     * Destructor.
     */
    virtual ~ExFileNotFound();

    Smp::String8 GetFileName() const noexcept override;

private:
    Smp::String8 _fileName;
};

}  // namespace kern
}  // namespace simph
#endif  // __simph_kern_FileNotFound_HPP__

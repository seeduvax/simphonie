/*
 * @file FileNotFound.hpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_kern_FileNotFound_HPP__
#define __simphonie_kern_FileNotFound_HPP__
#include "Smp/IObject.h"
#include "Smp/FileNotFound.h"
#include "simdeck/Exception.hpp"

namespace simphonie {
namespace kern {

/**
 *
 */
class ExFileNotFound : public simdeck::Exception, virtual public Smp::FileNotFound {
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
#endif  // __simphonie_kern_FileNotFound_HPP__

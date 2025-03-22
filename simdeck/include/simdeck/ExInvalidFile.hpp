/*
 * @file ExInvalidFile.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_ExInvalidFile_HPP__
#define __simdeck_ExInvalidFile_HPP__

#include "Smp/InvalidFile.h"
#include "simdeck/Exception.hpp"

namespace simdeck {

/**
 *
 */
class ExInvalidFile: public Exception, virtual public Smp::InvalidFile {
public:
    /**
     * Default constructor.
     */
    ExInvalidFile(const Smp::IObject* sender, 
                    Smp::String8 fileName,
                    Smp::String8 errorMsg);
    /**
     * Destructor.
     */
    virtual ~ExInvalidFile();

    // Smp::InvalidFile implementation
    Smp::String8 GetFileName() const noexcept;
    Smp::String8 GetErrorMessage() const noexcept;
private:
    std::string _fileName;
    std::string _errorMsg;
};

} // namespace simdeck
#endif // __simdeck_ExInvalidFile_HPP__

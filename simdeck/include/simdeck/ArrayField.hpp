/*
 * @file ArrayField.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_ArrayField_HPP__
#define __simdeck_ArrayField_HPP__

#include <ostream>
#include "Smp/IArrayField.h"

namespace simdeck {

///**
// *
// */
//class ArrayField {
//public:
//    /**
//     * Default constructor.
//     */
//    ArrayField();
//    /**
//     * Destructor.
//     */
//    virtual ~ArrayField();
//
//private:
//
//};

std::ostream& toprint(std::ostream& os, const Smp::IArrayField& obj);

} // namespace simdeck
#endif // __simdeck_ArrayField_HPP__

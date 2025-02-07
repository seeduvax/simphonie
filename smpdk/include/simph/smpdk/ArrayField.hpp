/*
 * @file ArrayField.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_smpdk_ArrayField_HPP__
#define __simph_smpdk_ArrayField_HPP__

#include <ostream>
#include "Smp/IArrayField.h"

namespace simph {
namespace smpdk {

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

}} // namespace simph::smpdk
#endif // __simph_smpdk_ArrayField_HPP__

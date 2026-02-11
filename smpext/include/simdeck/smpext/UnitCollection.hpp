/*
 * @file UnitCollection.hpp
 *
 * Copyright 2026 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_smpext_UnitCollection_HPP__
#define __simdeck_smpext_UnitCollection_HPP__

#include "simdeck/smpext/IUnit.hpp"

namespace simdeck {
namespace smpext {
    typedef Smp::ICollection<IUnit> UnitCollection;
}} // namespace simdeck::smpext
#endif // __simdeck_smpext_UnitCollection_HPP__

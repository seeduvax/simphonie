/*
 * @file Uuid.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */

#pragma once

#if defined(__clang__) && (__clang_major__ == 13)
// hack to be able to include Uuid.h with clang-13 without error on hash type.
#include <memory>
namespace Smp {
struct Uuid;
}
namespace std
{
    /// Specialisation of std::hash for UUIDs.
    /// Mandating that UUID is hashable ensures that it is possible
    /// to use it as key in unordered container.
    template <>
    struct hash<Smp::Uuid>
    {
        /// It calculates the hash of the Uuid argument.
        /// @param uuid the uuid for which the hash has to be calculated.
        /// @return Hash value for instance.
        size_t operator()(const Smp::Uuid& uuid) const;
    };
}

#define hash disabled_hash
#include <Smp/Uuid.h>
#undef hash

#else

#include <Smp/Uuid.h>

#endif
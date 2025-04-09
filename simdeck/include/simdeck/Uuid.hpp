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

#include <memory>
#include <array>
#include <cstdint>
#include <iosfwd>

#if defined(__clang__) && (__clang_major__ == 13) && defined(_LIBCPP_TEMPLATE_VIS)
// hack to be able to include Uuid.h with clang-13 on centos7 (llvm13) without error on hash type.
#ifndef SMP_UUID_H_
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
#endif // SMP_UUID_H_

#else

#include <Smp/Uuid.h>

#endif
/*
 * @file ICustomTypeRegistry.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_smpext_ICustomTypeRegistry_HPP__
#define __simdeck_smpext_ICustomTypeRegistry_HPP__

#include "Smp/Publication/ITypeRegistry.h"

namespace simdeck {
namespace smpext {

/**
 * Extends the SMP ITypeRegistry to enable addition of any type implementing
 * the SMP IType interface.
 * This may be used on model library load to register type handling opaque
 * custom type.
 * To do so, the related IType implementations shall implement the Publish
 * method as a kind of Field factory that create the related Field and publish
 * it as field directly (meaning not using recursively the void pointer publish
 * method that would recurse indefintely until stack overflow).
 * The actual IType::Publish methods appears to be some kind of survivance of
 * SMP2 and its use is not clear in the frame of ECSS SMP. To create the Field
 * properly, its parent shall be retrieved and there is no abvious and direct 
 * way to do it (no dedicated argument, and publication does not have specific
 * accessor to fetch it). One workaround is to temporarly publish a dummy
 * single byte field, get the parent from it, then remove this dummy field to
 * then create the right one using the same parent. An example of such trick is
 * avaialable in simdeck::Type::Publish implementation.
 */
class ICustomTypeRegistry: virtual public Smp::Publication::ITypeRegistry {
public:
    virtual ~ICustomTypeRegistry() noexcept = default;
    /**
     * Register a type to the registry.
     * From there, the type is own by the registry and will be deleted by
     * the registry on its own destruction.
     * @param type type to be registered.
     */
    virtual void AddType(Smp::Publication::IType* type) = 0;

private:

};

}} // namespace simdeck::smpext
#endif // __simdeck_smpext_ICustomTypeRegistry_HPP__

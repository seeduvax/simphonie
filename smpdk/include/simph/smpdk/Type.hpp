/*
 * @file Type.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_smpdk_Type_HPP__
#define __simph_smpdk_Type_HPP__
#include "Smp/Publication/IType.h"
#include "Smp/IPublication.h"
#include "simph/smpdk/Object.hpp"

namespace simph {
namespace smpdk {
using namespace simph::smpdk;

/**
 *
 */
class Type : public Object, virtual public Smp::Publication::IType {
public:
    /**
     * Default constructor.
     */
    Type(Smp::Uuid uuid, Smp::PrimitiveTypeKind kind, Smp::UInt64 typeSize, Smp::String8 name, Smp::String8 descr = "",
         Smp::IObject* parent = nullptr);
    /**
     * Destructor.
     */
    virtual ~Type();
    // Smp::Publication::IType implementation
    Smp::PrimitiveTypeKind GetPrimitiveTypeKind() const override;
    Smp::Uuid GetUuid() const override;
    Smp::IField* Publish(
        Smp::Publication::IPublishField* receiver,
        Smp::String8 name,
        Smp::String8 description,
        void* address,
        Smp::ViewKind view = Smp::ViewKind::VK_All,
        Smp::Bool state = true,
        Smp::Bool input = false,
        Smp::Bool output = false) override;
    inline Smp::UInt64 getSize() const {
        return _size;
    }

protected:
    void setSize(Smp::UInt64 newSize) {
        _size = newSize;
    }

private:
    Smp::Uuid _uuid;
    Smp::PrimitiveTypeKind _kind;
    Smp::UInt64 _size;
};

}  // namespace smpdk
}  // namespace simph
#endif  // __simph_smpdk_Type_HPP__

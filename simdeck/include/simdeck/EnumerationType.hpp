/*
 * @file EnumerationType.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_EnumerationType_HPP__
#define __simdeck_EnumerationType_HPP__
#include <vector>
#include "simdeck/Type.hpp"
#include "Smp/Publication/IEnumerationType.h"

namespace simdeck {

/**
 *
 */
class EnumerationType : public Type, virtual public Smp::Publication::IEnumerationType {
public:
    /**
     * Default constructor.
     */
    EnumerationType(Smp::Uuid uuid, Smp::PrimitiveTypeKind kind, Smp::String8 name, Smp::String8 descr = "",
                    Smp::IObject* parent = nullptr);
    /**
     * Destructor.
     */
    virtual ~EnumerationType();
    // Smp::Publication::IEnumerationType implementation
    void AddLiteral(Smp::String8 name, Smp::String8 description, Smp::Int32 value) override;
protected:
    Smp::IField* createField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::IComponent* parent,
        void* address,
        Smp::ViewKind view,
        Smp::Bool state,
        Smp::Bool input,
        Smp::Bool output) const override;
private:
    struct literal {
        Smp::String8 name;
        Smp::String8 description;
        Smp::Int32 value;
    };
    std::vector<struct literal> _literals;
};

}  // namespace simdeck
#endif  // __simdeck_EnumerationType_HPP__

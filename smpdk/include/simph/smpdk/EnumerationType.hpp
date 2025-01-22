/*
 * @file EnumerationType.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_smpdk_EnumerationType_HPP__
#define __simph_smpdk_EnumerationType_HPP__
#include <vector>
#include "Smp/Publication/IEnumerationType.h"
#include "simph/smpdk/Type.hpp"

namespace simph {
namespace smpdk {

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

private:
    struct literal {
        Smp::String8 name;
        Smp::String8 description;
        Smp::Int32 value;
    };
    std::vector<struct literal> _literals;
};

}  // namespace smpdk
}  // namespace simph
#endif  // __simph_smpdk_EnumerationType_HPP__

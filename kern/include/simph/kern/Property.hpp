/*
 * @file Property.hpp
 *
 * Copyright 2020 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_Property_HPP__
#define __simph_kern_Property_HPP__

#include "Smp/IProperty.h"
#include "simph/smpdk/Object.hpp"

namespace simph {
namespace kern {

/**
 *
 */
class Property : public simph::smpdk::Object, virtual public Smp::IProperty {
public:
    /**
     * Default constructor.
     */
    Property(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent, Smp::Publication::IType* type,
             Smp::AccessKind access, Smp::ViewKind view);
    /**
     * Destructor.
     */
    virtual ~Property();

    // Smp::IProperty implementation
    Smp::Publication::IType* GetType() const override;
    Smp::AccessKind GetAccess() const override;
    Smp::ViewKind GetView() const override;
    Smp::AnySimple GetValue() const override;
    void SetValue(Smp::AnySimple value) override;

private:
    Smp::Publication::IType* _type;
    Smp::AccessKind _access;
    Smp::ViewKind _view;
    Smp::AnySimple _value;
};

}  // namespace kern
}  // namespace simph
#endif  // __simph_kern_Property_HPP__

/*
 * @file Field.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_Field_HPP__
#define __simph_kern_Field_HPP__
#include "Smp/IField.h"
#include "simph/kern/Persist.hpp"

namespace simph {
	namespace kern {

/**
 *
 */
class Field: virtual public Persist, virtual public Smp::IField {
public:
    /**
     * Default constructor.
     */
    Field();
    /**
     * Destructor.
     */
    virtual ~Field();
    // Smp::IField implementation
    Smp::ViewKind GetView() const;
    Smp::Bool IsState() const;
    Smp::Bool IsInput() const;
    Smp::Bool IsOutput() const;
    const Smp::Publication::IType* GetType() const;
private:

};

}} // namespace simph::kern
#endif // __simph_kern_Field_HPP__

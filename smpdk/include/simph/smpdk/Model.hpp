/*
 * @file Model.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_smpdk_Model_HPP__
#define __simph_smpdk_Model_HPP__

#include "simph/smpdk/Component.hpp"
#include "Smp/IModel.h"

namespace simph {
namespace smpdk {

/**
 *
 */
class Model: public Component, virtual public Smp::IModel {
    typedef Component Parent;
public:
    /**
     * Default constructor.
     */
    Model(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent);
    /**
     * Destructor.
     */
    virtual ~Model();

private:

};

}} // namespace simph::smpdk
#endif // __simph_smpdk_Model_HPP__

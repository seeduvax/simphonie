/*
 * @file CompositeModel.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_smpdk_CompositeModel_HPP__
#define __simph_smpdk_CompositeModel_HPP__

#include "simph/smpdk/Model.hpp"
#include "simph/smpdk/Composite.hpp"

namespace simph {
namespace smpdk {

/**
 *
 */
class CompositeModel: public Model, virtual public Composite {
    typedef Model Parent;
public:
    /**
     * Default constructor.
     */
    CompositeModel(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent);
    /**
     * Destructor.
     */
    virtual ~CompositeModel();

private:

};

}} // namespace simph::smpdk
#endif // __simph_smpdk_CompositeModel_HPP__

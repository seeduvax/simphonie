/*
 * @file EPPModel.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_smpdk_EPPModel_HPP__
#define __simph_smpdk_EPPModel_HPP__

#include "simph/smpdk/Model.hpp"
#include "simph/smpdk/EntryPointPublisher.hpp"

namespace simph {
namespace smpdk {

/**
 *
 */
class EPPModel: public Model, virtual public EntryPointPublisher {
    typedef Model Parent;
public:
    /**
     * Default constructor.
     */
    EPPModel(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent);
    /**
     * Destructor.
     */
    virtual ~EPPModel();

private:

};

}} // namespace simph::smpdk
#endif // __simph_smpdk_EPPModel_HPP__

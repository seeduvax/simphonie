/*
 * @file AStepMdl.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_umdl_AStepMdl_HPP__
#define __simph_umdl_AStepMdl_HPP__
#include "Smp/IModel.h"
#include "Smp/IEntryPoint.h"
#include "Smp/IEntryPointPublisher.h"
#include "simph/kern/Component.hpp"


namespace simph {
	namespace umdl {

/**
 *
 */
class AStepMdl: public simph::kern::Component,
                public virtual Smp::IModel,
                public virtual Smp::IEntryPointPublisher {
public:
    /**
     * Default constructor.
     */
    AStepMdl(Smp::String8 name, Smp::String8 descr, 
                            Smp::IObject* parent=nullptr);
    /**
     * Destructor.
     */
    virtual ~AStepMdl();
    /**
     * Step function to implement to define the model's behavior
     */ 
    virtual void step()=0;
    // Smp::IEntryPointPublisher implementation
    const Smp::EntryPointCollection* GetEntryPoints() const;
    Smp::IEntryPoint* GetEntryPoint(Smp::String8 name) const;
private:
    Smp::IEntryPoint* _stepEp;
    simph::kern::Collection<Smp::IEntryPoint> _epList;
    Smp::ISimulator* _simulator;
};

}} // namespace simph::umdl
#endif // __simph_umdl_AStepMdl_HPP__

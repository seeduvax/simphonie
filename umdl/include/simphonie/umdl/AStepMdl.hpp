/*
 * @file AStepMdl.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_umdl_ArrayType_HPP__
#define __simphonie_umdl_ArrayType_HPP__
#include "Smp/IModel.h"
#include "simdeck/EPPModel.hpp"

namespace simphonie {
namespace umdl {

/**
 *
 */
class AStepMdl : public simdeck::EPPModel {
    typedef simdeck::EPPModel Parent;
public:
    /**
     * Default constructor.
     */
    AStepMdl(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent = nullptr);
    /**
     * Destructor.
     */
    virtual ~AStepMdl();
    /**
     * Step function to implement to define the model's behavior
     */
    virtual void step() = 0;
};

}  // namespace umdl
}  // namespace simph
#endif  // __simphonie_umdl_AStepMdl_HPP__

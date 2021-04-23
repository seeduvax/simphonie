/*
 * @file FallibleModel.hpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_FallibleModel_HPP__
#define __simph_kern_FallibleModel_HPP__
#include "Smp/ICollection.h"
#include "Smp/IFailure.h"
#include "Smp/IFallibleModel.h"
#include "simph/umdl/AStepMdl.hpp"

namespace simph {
namespace umdl {
/**
 *
 */
// TODO should derivate of AStepMdl?
class FallibleModel : public AStepMdl, virtual public Smp::IFallibleModel {
public:
    /**
     * Default constructor.
     */
    FallibleModel(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent = nullptr,
                  Smp::FailureCollection* failureList = nullptr);
    /**
     * Destructor.
     */
    ~FallibleModel();

    Smp::Bool IsFailed() const override;
    const Smp::FailureCollection* GetFailures() const override;
    Smp::IFailure* GetFailure(Smp::String8 name) const override;

private:
    Smp::ICollection<Smp::IFailure>* _failureList;
};

}  // namespace umdl
}  // namespace simph
#endif  // __simph_kern_FallibleModel_HPP__

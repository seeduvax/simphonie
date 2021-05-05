/*
 * @file FallibleModel.hpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_smpdk_FallibleModel_HPP__
#define __simph_smpdk_FallibleModel_HPP__

#include "Smp/IFailure.h"
#include "Smp/IFallibleModel.h"
#include "Smp/IModel.h"
#include "simph/smpdk/Collection.hpp"

namespace simph {
namespace smpdk {
/**
 *
 */
// TODO derivate IModel
class FallibleModel : virtual public Smp::IModel, virtual public Smp::IFallibleModel {
public:
    /**
     * Default constructor.
     */
    FallibleModel();
    /**
     * Destructor.
     */
    virtual ~FallibleModel();

    // IFaillibleModel implementation
    Smp::Bool IsFailed() const override;
    const Smp::FailureCollection* GetFailures() const override;
    Smp::IFailure* GetFailure(Smp::String8 name) const override;

protected:
    void AddFailure(Smp::IFailure* failure);

private:
    Collection<Smp::IFailure> _failureList;
};

}  // namespace smpdk
}  // namespace simph
#endif  // __simph_smpdk_FallibleModel_HPP__

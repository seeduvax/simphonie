/*
 * @file Failure.hpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_Failure_HPP__
#define __simph_kern_Failure_HPP__
#include "Smp/IFailure.h"
#include "simph/kern/Persist.hpp"

namespace simph {
namespace kern {
/**
 *
 */
class Failure : public Persist, virtual public Smp::IFailure {
public:
    /**
     * Default constructor.
     */
    // TODO Failure parent is the model where the failure can be apply (only)?
    Failure(Smp::String8 name, Smp::String8 descr = "", Smp::IObject* parent = nullptr);
    /**
     * Destructor.
     */
    virtual ~Failure();

    /// Sets the state of the failure to failed.
    void Fail() override;
    /// Sets the state of the failure to unfailed.
    void Unfail() override;

    /// Returns whether the failure's state is set to failed.
    /// @return  Returns true if the failure state is Failed, false
    ///          otherwise.
    Smp::Bool IsFailed() const override;

private:
    Smp::Bool _failed;

    void doFail();
    void doUnfail();
};

}  // namespace kern
}  // namespace simph
#endif  // __simph_kern_Failure_HPP__

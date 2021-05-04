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
    Failure(Smp::String8 name, Smp::String8 descr = "", Smp::IObject* parent = nullptr);
    /**
     * Destructor.
     */
    virtual ~Failure();

    void Fail() override;

    void Unfail() override;

    Smp::Bool IsFailed() const override;

protected:
    // TODO bool return or catch exception when failure application fail
    virtual void doFail();
    virtual void doUnfail();

private:
    Smp::Bool _failed;

};

}  // namespace kern
}  // namespace simph
#endif  // __simph_kern_Failure_HPP__

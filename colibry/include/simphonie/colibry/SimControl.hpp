/*
 * @file SimControl.hpp
 *
 * Copyright 2025 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_colibry_SimControl_HPP__
#define __simphonie_colibry_SimControl_HPP__

#include <memory>
#include "Smp/ISimulator.h"
#include "simdeck/EntryPointPublisher.hpp"
#include "simdeck/Service.hpp"
#include "sxeval/SXEval.hpp"

namespace simphonie {
namespace colibry {

class SimControl : public simdeck::Service, virtual public simdeck::EntryPointPublisher {
public:
    SimControl(Smp::String8 name, Smp::String8 descr = "", Smp::IObject* parent = nullptr);
    ~SimControl() = default;

protected:
    void publish(Smp::IPublication* receiver) override;
    void connect() override;

private:
    typedef double T;

    void _checkStopCondition();

    std::string _condition;
    std::unique_ptr<sxeval::SXEval<T> > _evaluator;
};

} /* namespace colibry */
} /* namespace simphonie */
#endif /* __simphonie_colibry_SimControl_HPP__ */

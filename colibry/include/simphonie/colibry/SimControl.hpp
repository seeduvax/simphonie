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
#include "simphonie/colibry/Tracker.hpp"

namespace simphonie {
namespace colibry {

class SimControl : public Tracker {
    typedef Tracker Parent;
public:
    SimControl(Smp::String8 name, Smp::String8 descr = "", Smp::IObject* parent = nullptr);
    ~SimControl();

protected:
    void onChange(Smp::Float64 value) override;
};

} /* namespace colibry */
} /* namespace simphonie */
#endif /* __simphonie_colibry_SimControl_HPP__ */

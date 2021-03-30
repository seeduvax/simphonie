/*
 * @file SmpIncrement.hpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_T_kern_SmpIncrement_HPP__
#define __simph_T_kern_SmpIncrement_HPP__

#include "simba/ASmpModel.hpp"

namespace simph {
	namespace T_kern {

class SmpIncrement: public simba::ASmpModel {
private:
    double _input;
    double _output;

protected:
    void publishModel() override;

public:
    SmpIncrement(Smp::String8 name,Smp::String8 description,Smp::IComposite* parent);
    virtual ~SmpIncrement();

    inline void step() {
        _output = _input + 1;
    }

    // set input value for tests
    inline void set(double input) {
        _input = input;
    };
};

}} // namespace simph::T_kern
#endif // __simph_T_kern_SmpIncrement_HPP__

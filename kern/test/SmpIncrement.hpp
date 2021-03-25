/*
 * @file SmpIncrement.hpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */

#include "simba/ASmpModel.hpp"

namespace test {

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
        std::cout << "output:" << _output << std::endl;
    }

    // set input value for tests
    inline void set(double input) {
        _input = input;
    };
};

}

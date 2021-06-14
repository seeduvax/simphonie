/*
 * @file Sampler.hpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_Sampler_HPP__
#define __simph_kern_Sampler_HPP__
#include <fstream>
#include <ostream>
#include <sstream>
#include "simph/kern/AStepMdl.hpp"
#include "simph/kern/Field.hpp"

namespace simph {
namespace kern {
/**
 *
 */
class Sampler : public AStepMdl {
public:
    /**
     * Default constructor.
     */
    Sampler(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent);
    /**
     * Destructor.
     */
    virtual ~Sampler();

    void recordField(simph::kern::Field* field);

    void step() override;

protected:
    void initColumn();

private:
    std::vector<simph::kern::Field*> _fields;
    std::ofstream _file;
    std::string _fileName;

    Smp::Bool _modeValue;
};

}  // namespace kern
}  // namespace simph
#endif  // __simph_kern_Sampler_HPP__

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
#include "simph/kern/Field.hpp"
#include "simph/umdl/AStepMdl.hpp"

namespace simph {
namespace umdl {
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

    void AddField(simph::kern::Field* field);

    void step() override;

private:
    // TODO use configure instead. expose a filename parameter ?
    void configure() override;

    std::vector<simph::kern::Field*> _fields;
    std::ofstream _file;
};

}  // namespace umdl
}  // namespace simph
#endif  // __simph_kern_Sampler_HPP__

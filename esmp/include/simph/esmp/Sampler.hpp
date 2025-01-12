/*
 * @file Sampler.hpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_esmp_Sampler_HPP__
#define __simph_esmp_Sampler_HPP__
#include <fstream>
#include <ostream>
#include <sstream>
#include "simph/smpdk/Component.hpp"
#include "simph/smpdk/IEntryPointPublisher.hpp"

namespace simph {
namespace esmp {
/**
 *
 */
class Sampler : public simph::smpdk::Component,
                public virtual simph::smpdk::IEntryPointPublisher {
    typedef simph::smpdk::Component Parent;
public:
    /**
     * Default constructor.
     */
    Sampler(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent);
    /**
     * Destructor.
     */
    virtual ~Sampler();

    void recordField(Smp::IField* field);

    void step();

protected:
    void initColumn();
    void Publish(Smp::IPublication* receiver) override;
    void Connect(Smp::ISimulator* sim) override;

private:
    std::vector<Smp::IField*> _fields;
    std::ofstream _file;
    std::string _fileName;

    Smp::Bool _modeValue;
};

}  // namespace esmp
}  // namespace simph
#endif  // __simph_esmp_Sampler_HPP__

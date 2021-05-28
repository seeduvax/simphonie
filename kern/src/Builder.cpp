
/*
 * @file Builder.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/Builder.hpp"
#include <iostream>
#include "simph/kern/Sampler.hpp"
#include "simph/kern/Simulator.hpp"
#include "simph/smpdk/Utils.hpp"
#include "simph/sys/Logger.hpp"

namespace simph {
namespace kern {
// --------------------------------------------------------------------
// ..........................................................
Builder::Builder(Smp::IObject* parent) : Parent("Builder", "descr", parent) {}

// ..........................................................
Builder::~Builder() {}

// ..........................................................
void Builder::AddLoadSmpModelCfg(LoadSmpModelCfg cfg) {
    _loadSmpModelCfgs.push_back(cfg);
}

// ..........................................................
void Builder::AddLoadConnectionCfg(LoadConnectionCfg cfg) {
    _loadConnectionCfg.push_back(cfg);
}

// ..........................................................
void Builder::AddLoadInitCfg(LoadInitCfg cfg) {
    _loadInitCfg.push_back(cfg);
}

// ..........................................................
void Builder::AddLoadSamplerCfg(LoadSamplerCfg cfg) {
    _loadSamplerCfg.push_back(cfg);
}

// ..........................................................
void Builder::AddLoadScheduleCfg(LoadScheduleCfg cfg) {
    _loadScheduleCfg.push_back(cfg);
}

// ..........................................................
void Builder::AddLoadParamCfg(LoadParamCfg cfg) {
    _loadParamCfg.push_back(cfg);
}

// ..........................................................
void Builder::publish(Smp::IPublication* receiver) {
    auto sim = getSimulator();
    // publish samplers
    for (auto cfg : _loadSamplerCfg) {
        simph::kern::Sampler sampler(simph::kern::Sampler(cfg.name.c_str(), cfg.description.c_str(), sim));
        sim->AddModel(&sampler);
    }
    // publish models
    for (auto cfg : _loadSmpModelCfgs) {
        sim->LoadLibrary(cfg.library.c_str());
        auto simk = dynamic_cast<kern::Simulator*>(sim);
        auto c = sim->CreateInstance(simph::smpdk::Utils::generateUuid(cfg.type.c_str()), cfg.name.c_str(),
                                     cfg.description.c_str(), sim);
        if (c == nullptr) {
            c = simk->createSmpModel(cfg.type.c_str(), cfg.name.c_str(), cfg.description.c_str());
            if (c == nullptr) {
                std::stringstream ss;
                ss << "Can't create instance " << cfg.library << " " << cfg.name << " " << cfg.type;
                throw std::runtime_error(ss.str());
            }
        }
    }
}

// ..........................................................
void Builder::connect() {
    // load EP sampler
    for (auto cfg : _loadSamplerCfg) {
        auto sampler =
            dynamic_cast<simph::kern::Sampler*>(getSimulator()->GetResolver()->ResolveAbsolute(cfg.name.c_str()));
        for (auto ep : cfg.fields) {
            sampler->AddField(
                dynamic_cast<simph::kern::Field*>(getSimulator()->GetResolver()->ResolveAbsolute(ep.c_str())));
        }
    }
    // loadInit
    for (auto cfg : _loadInitCfg) {
        auto field = dynamic_cast<Smp::ISimpleField*>(getSimulator()->GetResolver()->ResolveAbsolute(cfg.path.c_str()));
        if (field != nullptr) {
            LOGI("Setting " << field->GetName() << " 's value");  //<< cfg.value)
            field->SetValue(cfg.value);
        }
        else {
            std::stringstream ss;
            ss << "Unknown field with path : " << cfg.path;
            throw std::runtime_error(ss.str());
        }
    }

    // loadConnect
    for (auto cfg : _loadConnectionCfg) {
        auto output_field =
            dynamic_cast<Smp::IDataflowField*>(getSimulator()->GetResolver()->ResolveAbsolute(cfg.from.c_str()));
        auto input_field =
            dynamic_cast<Smp::IDataflowField*>(getSimulator()->GetResolver()->ResolveAbsolute(cfg.to.c_str()));

        if (input_field == nullptr) {
            std::stringstream ss;
            ss << "Input field not find (" << input_field << ")";
            throw std::runtime_error(ss.str().c_str());
        }
        if (output_field == nullptr) {
            std::stringstream ss;
            ss << "Output field not find (" << output_field << ")";
            throw std::runtime_error(ss.str().c_str());
        }

        LOGI("Connecting output " << output_field->GetName() << " to " << input_field->GetName());
        output_field->Connect(input_field);
    }

    // loadSchedule
    for (auto cfg : _loadScheduleCfg) {
        auto entryPoint =
            dynamic_cast<Smp::IEntryPoint*>(getSimulator()->GetResolver()->ResolveAbsolute(cfg.path.c_str()));
        if (entryPoint != nullptr) {
            LOGI("Scheduling entrypoint" << entryPoint->GetName() << ", period=" << cfg.period);
            getSimulator()->GetScheduler()->AddSimulationTimeEvent(entryPoint, 0, cfg.period, -1);
        }
        else {
            std::stringstream ss;
            ss << "Unknown entrypoint with path : " << cfg.path;
            throw std::runtime_error(ss.str());
        }
    }
};

// ..........................................................
void Builder::configure() {
    for (auto cfg : _loadParamCfg) {
        auto field = dynamic_cast<Smp::ISimpleField*>(getSimulator()->GetResolver()->ResolveAbsolute(cfg.path.c_str()));
        field->SetValue(cfg.value);
    }
}

}  // namespace kern
}  // namespace simph

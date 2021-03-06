
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
    // publish models
    for (auto cfg : _loadSmpModelCfgs) {
        sim->LoadLibrary(cfg.library.c_str());
        auto simk = dynamic_cast<simph::kern::Simulator*>(sim);
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
    // publish samplers
    for (auto cfg : _loadSamplerCfg) {
        sim->LoadLibrary("libsimph_kern.so");
        auto simk = dynamic_cast<simph::kern::Simulator*>(sim);
        auto sampler = dynamic_cast<simph::kern::Sampler*>(simk->CreateInstance(
            simph::smpdk::Utils::generateUuid("Sampler"), cfg.name.c_str(), cfg.description.c_str(), sim));
        _samplers.push_back(sampler);
    }
}

// ..........................................................
void Builder::connect() {
    // load EP sampler
    for (auto cfg : _loadSamplerCfg) {
        auto sampler =
            dynamic_cast<simph::kern::Sampler*>(getSimulator()->GetResolver()->ResolveAbsolute(cfg.name.c_str()));
        for (auto ep : cfg.fields) {
            auto field = dynamic_cast<simph::kern::Field*>(getSimulator()->GetResolver()->ResolveAbsolute(ep.c_str()));
            sampler->recordField(field);
        }
        // getSimulator()->GetScheduler()->AddSimulationTimeEvent(dynamic_cast<Smp::IEntryPoint*>(getSimulator()->GetResolver()->ResolveRelative("step",
        // sampler)), 0, cfg.period, -1);
        auto entryPoint =
            dynamic_cast<Smp::IEntryPoint*>(getSimulator()->GetResolver()->ResolveRelative("step", sampler));
        getSimulator()->GetScheduler()->AddSimulationTimeEvent(entryPoint, 0, 5000000, -1);
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
    uint16_t k = 0;
    for (auto cfg : _loadSamplerCfg) {
        auto mode = dynamic_cast<kern::Field*>(_samplers[k]->GetField("mode"));
        mode->SetValue(cfg.mode);
        for (auto fieldPath : cfg.fields) {
            auto field = dynamic_cast<kern::Field*>(getSimulator()->GetResolver()->ResolveAbsolute(fieldPath.c_str()));
            if (field != nullptr)
                _samplers[k]->recordField(field);
        }
        k += 1;
    }
    for (auto cfg : _loadParamCfg) {
        auto field = dynamic_cast<Smp::ISimpleField*>(getSimulator()->GetResolver()->ResolveAbsolute(cfg.path.c_str()));
        field->SetValue(cfg.value);
    }
}

}  // namespace kern
}  // namespace simph

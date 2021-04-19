/*
 * @file Builder.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <sstream>
#include <string>

#include "Smp/AnySimple.h"
#include "Smp/IDataflowField.h"
#include "Smp/IEntryPointPublisher.h"
#include "Smp/IService.h"
#include "Smp/ISimpleField.h"
#include "Smp/ISimulator.h"
#include "simph/smpdk/Component.hpp"

namespace simph {
namespace kern {
class Builder : public simph::smpdk::Component, virtual public Smp::IService {
    typedef simph::smpdk::Component Parent;

public:
    Builder(Smp::IObject* parent);
    virtual ~Builder();

    struct LoadSmpModelCfg {
        std::string library;
        std::string type;
        std::string name;
        std::string description;
    };
    struct LoadConnectionCfg {
        std::string from;
        std::string to;
    };
    struct LoadInitCfg {
        std::string path;
        Smp::AnySimple value;
    };
    struct LoadScheduleCfg {
        std::string path;
        uint32_t period;
    };
    struct LoadParamCfg {
        std::string path;
        Smp::AnySimple value;
    };

    void AddLoadSmpModelCfg(LoadSmpModelCfg cfg);
    void AddLoadConnectionCfg(LoadConnectionCfg cfg);
    void AddLoadInitCfg(LoadInitCfg cfg);
    void AddLoadScheduleCfg(LoadScheduleCfg cfg);
    void AddLoadParamCfg(LoadParamCfg cfg);
    void publish(Smp::IPublication* receiver) override;
    void connect() override;
    void configure() override;

private:
    std::vector<LoadSmpModelCfg> _loadSmpModelCfgs;
    std::vector<LoadConnectionCfg> _loadConnectionCfg;
    std::vector<LoadInitCfg> _loadInitCfg;
    std::vector<LoadScheduleCfg> _loadScheduleCfg;
    std::vector<LoadParamCfg> _loadParamCfg;
};

}  // namespace kern
}  // namespace simph

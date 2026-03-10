/*
 * @file Simulator.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_kern_Simulator_HPP__
#define __simphonie_kern_Simulator_HPP__
#include "Smp/ISimulator.h"
#include "Smp/Publication/ITypeRegistry.h"
#include "simdeck/Composite.hpp"
#include "simphonie/sys/DLib.hpp"
#include "Smp/Services/IResolver.h"
#include <thread>
#include <mutex>
#include <condition_variable>

namespace simphonie {
namespace kern {
using namespace simdeck;

class Resolver;
class Scheduler;
/**
 *
 */
class Simulator : public Composite, public virtual Smp::ISimulator {
    typedef Composite Parent;
public:
    /**
     * Default constructor.
     */
    Simulator(Smp::String8 name = "Simphonie", Smp::String8 descr = "", Smp::IObject* parent = nullptr);
    /**
     * Destructor.
     */
    virtual ~Simulator();
    // Smp::ISimulator implementation
    void Initialise() override;
    void Publish() override;
    void Configure() override;
    void Connect() override;
    void Run() override;
    void Hold(Smp::Bool immediate) override;
    void Store(Smp::String8 filename) override;
    void Restore(Smp::String8 filename) override;
    void Reconnect(Smp::IComponent* root) override;
    void Exit() override;
    void Abort() override;
    Smp::SimulatorStateKind GetState() const override;
    void AddInitEntryPoint(Smp::IEntryPoint* entryPoint) override;
    void AddModel(Smp::IModel* model) override;
    void AddService(Smp::IService* service) override;
    Smp::IService* GetService(Smp::String8 name) const override;
    Smp::Services::ILogger* GetLogger() const override;
    Smp::Services::ITimeKeeper* GetTimeKeeper() const override;
    Smp::Services::IScheduler* GetScheduler() const override;
    Smp::Services::IEventManager* GetEventManager() const override;
    Smp::Services::IResolver* GetResolver() const override;
    Smp::Services::ILinkRegistry* GetLinkRegistry() const override;
    void RegisterFactory(Smp::IFactory* componentFactory) override;
    Smp::IComponent* CreateInstance(Smp::Uuid uuid, Smp::String8 name, Smp::String8 description,
                                    Smp::IComposite* parent) override;
    Smp::IFactory* GetFactory(Smp::Uuid uuid) const override;
    const Smp::FactoryCollection* GetFactories() const override;
    Smp::Publication::ITypeRegistry* GetTypeRegistry() const override;
    void LoadLibrary(
            Smp::String8 libraryPath,
            Smp::LibraryLoadingFlag loadFlag = Smp::LibraryLoadingFlag::LLF_Auto) override;

private:
    Smp::SimulatorStateKind _state;
    Collection<Smp::IEntryPoint> _initEntryPoints;
    Smp::IContainer* _models;
    Smp::IContainer* _services;
    std::vector<std::unique_ptr<Smp::IFactory>> _ownedFactories;
    Collection<Smp::IFactory> _compFactories;
    Smp::Services::ILogger* _logger=nullptr;
    Smp::Services::IScheduler* _scheduler=nullptr;
    Smp::Services::ITimeKeeper* _timeKeeper=nullptr;
    Smp::Services::IEventManager* _eventMgr=nullptr;
    Smp::Services::ILinkRegistry* _linkRegistry=nullptr;
    Smp::Publication::ITypeRegistry* _typeRegistry=nullptr;
    Smp::Services::IResolver* _resolver=nullptr;
    std::vector<simphonie::sys::DLib*> _libs;
    std::vector<Smp::IPublication*> _publications;

    void doPublish(Smp::IComponent* comp);
    void doConfigure(Smp::IComponent* comp);
    void doConnect(Smp::IComponent* comp);
    void doDisconnect(Smp::IComponent* comp);
    void setState(Smp::SimulatorStateKind newState);
    bool checkState(Smp::String8 opName, Smp::SimulatorStateKind expState);
    /**
     * Entry point - start the simulation.
     * On Run(), The simulator schedule itself this entry point once to the
     * scheduler in order to have some action performed from the scheduler
     * thrend on simulation start.
     */  
    void epStart();
    Smp::IEntryPoint* _epStart=nullptr;
    /**
     * Entry Point - stop the simulation.
     * On Hold(), the simulator schedule itself this entry point, immediate or
     * with smallest delay in conformance with the immeadiate argument of Hold
     * to let the simulation end be triggered at the right time by the scheduler
     * thread.
     */
    void epStop();
    Smp::IEntryPoint* _epStop=nullptr;
    std::thread::id _schedulerThreadId;
    // for stop simulation synchro handling.
    std::mutex _mutex;
    std::condition_variable _monitor;
    bool _waitingStop=false;


    template<typename T>
    void overrideService(T** target,Smp::IComponent* service) {
        auto s=dynamic_cast<T*>(service);
        if (s!=nullptr) {
            _services->DeleteComponent(*target);
            *target=s;
        }
    }
};

}  // namespace kern
}  // namespace simph
#endif  // __simphonie_kern_Simulator_HPP__

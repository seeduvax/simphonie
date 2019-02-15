/*
 * @file Simulator.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_Simulator_HPP__
#define __simph_kern_Simulator_HPP__
#include "Smp/ISimulator.h"
#include "simph/kern/Composite.hpp"
#include "Smp/Publication/ITypeRegistry.h"

namespace simph {
	namespace kern {

/**
 *
 */
class Simulator: public virtual Composite, 
		public virtual Smp::ISimulator {
public:
    /**
     * Default constructor.
     */
    Simulator(Smp::String8 name="Simphonie",
            Smp::String8 descr="",
            Smp::IObject* parent=nullptr);
    /**
     * Destructor.
     */
    virtual ~Simulator();

    void Initialise();
    void Publish();
    void Configure();
    void Connect();
    void Run();
    void Hold();
    void Store(Smp::String8 filename);
    void Restore(Smp::String8 filename);
    void Reconnect(Smp::IComponent* root);
    void Exit();
    void Abort();
    Smp::SimulatorStateKind GetState() const;
    void AddInitEntryPoint(Smp::IEntryPoint* entryPoint);
    void AddModel(Smp::IModel* model);
    void AddService(Smp::IService* service);
    Smp::IService* GetService(Smp::String8 name) const;
    Smp::Services::ILogger* GetLogger() const;
    Smp::Services::ITimeKeeper* GetTimeKeeper() const;
    Smp::Services::IScheduler* GetScheduler() const;
    Smp::Services::IEventManager* GetEventManager() const;
    Smp::Services::IResolver* GetResolver() const;
    Smp::Services::ILinkRegistry* GetLinkRegistry() const;
    void RegisterFactory(Smp::IFactory* componentFactory);
    Smp::IComponent* CreateInstance(Smp::Uuid uuid,
		       Smp::String8 name,
		       Smp::String8 description,
		       Smp::IComposite* parent);
    Smp::IFactory* GetFactory(Smp::Uuid uuid) const;
private:
    Smp::SimulatorStateKind _state;
    Collection<Smp::IEntryPoint> _initEntryPoints;
    Smp::IContainer* _models;
    Smp::IContainer* _services;
    Collection<Smp::IFactory> _compFactories;
    Smp::Services::ILogger* _logger;
    Smp::Services::IScheduler* _scheduler;
    Smp::Services::ITimeKeeper* _timeKeeper;
    Smp::Publication::ITypeRegistry* _typeRegistry;
    void publish(Smp::IComponent* comp);
    void configure(Smp::IComponent* comp);
    void connect(Smp::IComponent* comp);
    void setState(Smp::SimulatorStateKind newState);
};

}} // namespace simph::kern
#endif // __simph_kern_Simulator_HPP__

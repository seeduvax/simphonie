-- TODO find why h5 recorder fails randomly on very few specific machine.
-- thread safe issue ? Pointer init issue ?

s=require "simphonie_lua"
sim2=s.CreateSimulator({
    name="MySim2",
    lib="simphonie_kern",
    libraries={
        "simphonie_umdl",
        "simphonie_colibry",
        "simphonie_lua",
        "simphonie_mt",
    },
    components={
        inc1Slave={type="simphonie::umdl::SmpIncrement"},
        inc2Slave={type="simphonie::umdl::SmpIncrement"},
--        recorder={type="simphonie::colibry::FieldRecorderHDF5",
--            filePath="myRecSim2.h5"},
        slave={type="simphonie::mt::SimSyncSlave"},
    },
    connections={
--        ["recorder/port"]={
--            "inc1Slave/input",
--            "inc2Slave/output",
--        },
        ["slave/inputs"]={
            "inc1Slave/input",
        },
        ["slave/outputs"]={
            "inc2Slave/output",
        },
    },
    schedule={
        {name="inc1Slave/step", cycleTime_ms=25, offset_ms=20},
        {name="inc2Slave/step", cycleTime_ms=50},
--        {name="recorder/step", cycleTime_ms=25},
        {name="slave/sync", cycleTime_ms=25},
    }
})
sim=s.CreateSimulator({
    name="MySim",
    lib="simphonie_kern",
    libraries={
        "simphonie_umdl",
        "simphonie_colibry",
        "simphonie_lua",
        "simphonie_mt",
        "simphonie_rest",
    },
    components={
        webserver={type="simphonie::rest::RestService"},
        ctrl={type="simphonie::colibry::SimControl",
            description="Auto stop the simulation when stop condition is reached.",
            Expression="(> SmpIncrementEvent 10)"
        },
        recorderCsv={type="simphonie::colibry::FieldRecorderCsv",
            filePath="myRec.csv"},
--        recorderH5={type="simphonie::colibry::FieldRecorderHDF5",
--            filePath="myRec.h5"},
        logger={type="simphonie::kern::Logger",
            Backends={
                loggerFile={type="simphonie::kern::LoggerFile",
                    filePath="myLogger.log"},
                loggerOStream={type="simphonie::kern::LoggerOStream"}
            }
        },
        schedulerTracker={type="simphonie::colibry::SchedulerTracker"},
        sync={type="simphonie::colibry::Synchronizer", period=25000000},
        master={type="simphonie::mt::SimSyncMaster", slave=sim2.slave},
        inc1={type="simphonie::umdl::SmpIncrement",
            Children={
                inc11={type="simphonie::umdl::SmpIncrement", description="to check sub component."}
            }
        },
        inc2={type="simphonie::umdl::SmpIncrement"},-- simdeck::C::C2SMPBridge<&SmpIncrementC>"},
        inc3={type="simphonie::umdl::SmpIncrement"},
        inc4={type="simphonie::umdl::SmpIncrement"},
        ["MyLuaModel=test/model.lua"]={type="simphonie::lua::LuaModel"},
    },
    connections={
        ["inc2/input"]="inc1/output",
        ["inc1/input"]="inc1/output", -- loop to really create a counter
        ["recorderCsv/port"]={
            "inc2/input",
            "inc1/output",
            "MyLuaModel/dblvect",
            "MyLuaModel/cpt",
        },
--        ["recorderH5/port"]={
--            "inc2/input",
--            "inc1/output",
--            "MyLuaModel/dblvect",
--            "MyLuaModel/cpt"
--        },
        ["master/inputs"]={
            "inc2/input",
        },
        ["master/outputs"]={
            "inc1/output",
        },
        inc2 = "inc1" -- just ro test link registry
    },
    schedule={
        {name="inc1/step", cycleTime_ms=25, simTime_ms=20},
        {name="inc2/step", startEvent="TheEvent", cycleTime_ms=50},
        {name="recorderCsv/step", cycleTime_ms=50},
--        {name="recorderH5/step", cycleTime_ms=50},
        {name="master/sync", cycleTime_ms=50, simTime_ms=20},
        {name="MyLuaModel/step", cycleTime_ms=50},
        ["inc4/step"]={startEvent="TheEvent", cycleTime_ms=30, simTime_ms=20, active=false}
    }
})

sim:Run()
print("Simulator name: "..sim.Name)
tk=sim:GetTimeKeeper()
print("Timekeeper simulation time: "..tk:GetSimulationTime())
print("Timekeeper state: "..tk.State)
sched=sim:GetScheduler()
print("Scheduler name: "..sched.Name)
res=sim:GetResolver()
print("plop")
print("resolver name: "..res.Name)
print("timekeeper state / simulation time:"..tk.State.."/"..tk:GetSimulationTime())
sim.testTK=tk;
print("scheduler description: "..sim:GetScheduler().Description)
print("scheduler state: "..sim:GetScheduler().State)

sim:CreateComponent("simphonie::umdl::SmpIncrement","inc","")
sim.inc:CreateChild("simphonie::umdl::SmpIncrement","Children","subinc","")
sim:Publish()
sim:Configure()
sim:Connect()
sim.inc.output:Connect(sim.inc.subinc.input)

sim.Resolver.dump:Execute()
print("simulator state "..sim.State)

sim.inc1.input.Value=10
sim.inc1.step:Execute()
sim.recorderCsv.step:Execute()
--sim.recorderH5.step:Execute()
sim.inc1.step:Execute()
sim.recorderCsv.step:Execute()
--sim.recorderH5.step:Execute()
sim.inc1.step:Execute()
sim.recorderCsv.step:Execute()
--sim.recorderH5.step:Execute()
front=s.CreateFront(sim)
front:Run()
front:WaitStandby(0)

sim.ctrl.Expression.Value = "(and (> (sqrt /TimeKeeper/simTime) 1234.0) (> /inc1/output 20.0))"
sim.ctrl.Init:Execute()
eventId=sim:GetEventManager():QueryEventId("TheEvent")
sim:GetEventManager():Emit(eventId, true)

front:Run()
front:WaitStandby(0)

sim:Store("mySimu.cp")
-- sim:Restore("mySimu.cp")

sim.schedulerTracker.logStats:Execute()
print("Nb. of Error logs: "..sim.logger.ErrorCounter.Value)
print("Nb. of Warning logs: "..sim.logger.WarningCounter.Value)
print("Nb. of Event logs: "..sim.logger.EventCounter.Value)
print("Nb. of Information logs: "..sim.logger.InformationCounter.Value)
print("Nb. of Debug logs: "..sim.logger.DebugCounter.Value)
print("Nb. of logs: "..sim.logger.Counter.Value)

s.DisposeFront(front)
s.DisposeSimulator(sim2)
-- do not dispose sim because cppunit test case will do some assert on it.

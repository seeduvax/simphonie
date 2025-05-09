s=require "simphonie_lua"
sim=s.Simphonie.Simulator.new("luaSim")
sim:setConfiguration({
    libraries={
        "simphonie_umdl",
        "simphonie_colibry"
    },
    components={
        ctrl={type="simphonie::colibry::SimControl",
            description="Auto stop the simulation when stop condition is reached.",
            condition="(and (> (sqrt /TimeKeeper/simTime) 1234.0) (> /inc1/output 30.0))"
        },
        recorder={type="simphonie::colibry::FieldRecorderCsv",
            filePath="myRec.csv"},
        logger={type="simphonie::kern::Logger",
            Backends={
                loggerFile={type="simphonie::kern::LoggerFile",
                    filePath="myLogger.log"},
                loggerOStream={type="simphonie::kern::LoggerOStream"}
            }
        },
        inc1={type="simphonie::umdl::SmpIncrement",
            Children={
                inc11={type="simphonie::umdl::SmpIncrement", description="to check sub component."}
            }
        },
        inc2={type="simphonie::umdl::SmpIncrement"},
    },
    connections={
        ["inc2/input"]="inc1/output",
        ["inc1/input"]="inc1/output", -- loop to really create a counter
        ["recorder/port"]={
            "inc2/input",
            "inc1/output"
        },
        inc2 = "inc1" -- just ro test link registry
    },
    schedule={
        ["inc1/step"]={cycleTime_ms=250},
        ["inc2/step"]={cycleTime_ms=500},
        ["recorder/step"]={cycleTime_ms=500}
    }
})
sim:Run()
print("Simulator name: "..sim.Name)
sim:LoadLibrary("simphonie_umdl",0)
sim:LoadLibrary("simphonie_colibry",0)
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
print("scheduler description: "..sim.Scheduler.Description)
print("scheduler state: "..sim.Scheduler.State)

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
sim.recorder.step:Execute()
sim.inc1.step:Execute()
sim.recorder.step:Execute()
sim.inc1.step:Execute()
sim.recorder.step:Execute()
sim:Run()

-- this is a bad way to wait the simulation is completed.
-- it is good enough waiting for more specific feature for that.
while sim.State~=3 do
end

print("Nb. of Error logs: "..sim.logger.ErrorCounter.Value)
print("Nb. of Warning logs: "..sim.logger.WarningCounter.Value)
print("Nb. of Event logs: "..sim.logger.EventCounter.Value)
print("Nb. of Information logs: "..sim.logger.InformationCounter.Value)
print("Nb. of Debug logs: "..sim.logger.DebugCounter.Value)
print("Nb. of logs: "..sim.logger.Counter.Value)

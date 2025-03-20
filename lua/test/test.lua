s=require "simphonie_lua"
sim=s.Simphonie.Simulator.new("luaSim")
sim:setConfiguration({
    components={
        inc1={type="simphonie::umdl::SmpIncrement",description=""},
        inc2={type="simphonie::umdl::SmpIncrement",description=""},
    }
})
sim:Run()
print("Simulator name: "..sim.Name)
sim:LoadLibrary("simphonie_umdl",0)
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
print("simulator state "..sim.State)

sim:CreateComponent("simphonie::umdl::SmpIncrement","inc","")
sim.inc:CreateChild("simphonie::umdl::SmpIncrement","children","subinc","")
sim:Publish()
sim.inc.output:Connect(sim.inc.subinc.input)

sim.Resolver.dump:Execute()

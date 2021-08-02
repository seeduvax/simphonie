s=require "libsimph_lua"
sim=s.Simphonie.Simulator.new("luaSim")
sim:Run()
print("Plop: "..sim.name)
--sim:LoadLibrary("simph_umdl")
tk=sim:GetTimeKeeper()
print("Plop: "..tk:GetSimulationTime())
print("Plop: "..tk:GetState())
sched=sim:GetScheduler()
print("Plop: "..sched.name)
res=sim:GetResolver()
print("Plip: "..res.name)
print("hhh:"..sim.kk:GetState().."/"..sim.kk:GetSimulationTime())
sim.testTK=tk;

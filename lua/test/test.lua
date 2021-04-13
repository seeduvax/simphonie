s=require "libsimph_lua"
sim=s.Simphonie.Simulator.new("luaSim")
sim:Run()
print("Plop: "..sim.name)
--sim:LoadLibrary("simph_umdl")
tk=sim:GetTimeKeeper()
print("Plop: "..tk:GetSimulationTime())
print("Plop: "..tk:GetState())
print("hhh:"..sim.kk:GetState().."/"..sim.kk:GetSimulationTime())
sim.testTK=tk;

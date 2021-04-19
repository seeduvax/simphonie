s=require 'libsimph_lua'

sim = s.Simphonie.Simulator.new("luaSim")

-- With Andre's help
sim:LoadLibrary("libsimph_umdl.so")
sim:createSmpModel("SmpIncrement","mySmpIncrementMdl","description")

sim:Publish()
sim:Configure()
sim:Connect()


sim:connect("mySmpIncrementMdl/output","mySmpIncrementMdl/input")
sim:schedule("mySmpIncrementMdl","step", 1000000)

print(sim.mySmpIncrementMdl)
print(sim.mySmpIncrementMdl.name)
print(sim.mySmpIncrementMdl.type)
a = sim.mySmpIncrementMdl
a:GetFields()

---- step 10s
sim:GetScheduler():step(10000000)
--
---- check
--print("====2")
--assert(sim:getData("mySmpIncrementMdl/output")==11)
--assert(sim:getData("mySmpIncrementMdl/input")==11)
--
--print ("= OK")

require 'libsimph_lua'

sim = Simulator()

-- With Andre's help
sim:loadSmpLib("libsimph_umdl.so")
sim:createSmpModel("SmpIncrement","mySmpIncrementMdl","description")

sim:Publish()
sim:Configure()
sim:Connect()

sim:connect("mySmpIncrementMdl/output","mySmpIncrementMdl/input")
sim:schedule("mySmpIncrementMdl","step", 1000000)

-- step 10s
sim:stepTime(10000000)

-- check
assert(sim:getData("mySmpIncrementMdl/output")==11)
assert(sim:getData("mySmpIncrementMdl/input")==11)

print ("= OK")
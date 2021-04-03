package.cpath="../build/Debian_10_x86_64/debug/lib/?.so"


require 'libsimph_lua'

sim = Simulator()

-- With Andre's help
sim:loadSmpLib("libsimph_T_kern.so")
sim:createSmpModel("simph_T_kern_SmpIncrement","mySmpIncrementMdl","description")

sim:Publish()
sim:Configure()
sim:Connect()

sim:connect("mySmpIncrementMdl/output","mySmpIncrementMdl/input")
sim:schedule("mySmpIncrementMdl","step", 1000000)
-- check


-- step 10s
sim:stepTime(10000000)

-- check
assert(sim:getData("mySmpIncrementMdl/output")==11)
assert(sim:getData("mySmpIncrementMdl/input")==11)

--  
--  (TODO with A. Astyl) sim:createModel("library", uuid", )
--
--  sim:Publish
--  ...
--
--
--
--  sim:Initialise()


-- last-1.9/stgshell/src/STGShell.cpp
-- retourner object de type LuaRef
-- ligne 94

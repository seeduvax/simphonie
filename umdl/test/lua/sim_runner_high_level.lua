s = require 'libsimph_lua'

sim = s.Simphonie.Simulator.new("luaSim")

sim:setConfiguration {
        models = { 
                { library = "libsimph_umdl.so", class = "SmpIncrement", name = "mySmpIncrementMdl", description="description" }
        },
        params= {},
        connections = {
                { from = "mySmpIncrementMdl/output", to = "mySmpIncrementMdl/input" }
        },
        inits = {
                { path = "mySmpIncrementMdl/input", value = 1 },
        },
        schedules = {
                { path = "mySmpIncrementMdl/step", period = 1000000 },
        }
}

sim:Publish()
sim:Configure()
sim:Connect()

-- run
sim:GetScheduler():step(10000000)

-- check
--assert(sim:getData("mySmpIncrementMdl/output")==11)
--assert(sim:getData("mySmpIncrementMdl/input")==11)

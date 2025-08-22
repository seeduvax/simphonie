s=require "simphonie_lua"
sim=s.CreateSimulator({
    name="MySim",
    lib="simphonie_kern",
    libraries={
        "simphonie_lua",
        "simphonie_umdl",
    },
    components={
        inc1={type="simphonie::umdl::SmpIncrement"},
        inc2={type="simphonie::umdl::SmpIncrement"},
    },
    connections={
        ["inc2/input"]="inc1/output",
        ["inc1/input"]="inc1/output",
    },
    schedule={
        {name="inc1/step", cycleTime_ms=25, offset_ms=20},
        {name="inc2/step", cycleTime_ms=50},
    }
})
sim:Publish()
sim:Configure()
sim:Connect()

return sim

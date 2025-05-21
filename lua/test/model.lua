model={
    publish=function(self)
        self:Publish({
            dblvectin={type="float64", input=true, size=3},
            cpt={type="int64", output=true, } 
        })
        self.cpt=42
        self:AddEntryPoint("step", "main model step",function (self)
            self.cpt = self.cpt + 1
        end)
    end,
    configure=function(self)
        print(":::::: Configure from lua") 
        print(self.plopVar)
    end,
    connect=function(self)
        print(":::::: Connect from lua") 
    end
}

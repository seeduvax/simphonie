model={
    publish=function(self)
        print(":::::: Publish form lua ")
        print(type(self)) 
        print(self.Name) 
        print("plop")
        self:AddEntryPoint("step", "main model step",function (self)
            print(":::::: model step from lua model "..self.Name)
            print(self.plopVar)
        end)
        self:PublishInt("plopVar")
        self.plopVar=42
    end,
    configure=function(self)
        print(":::::: Configure from lua") 
        print(self.plopVar)
    end,
    connect=function(self)
        print(":::::: Connect from lua") 
    end,
}

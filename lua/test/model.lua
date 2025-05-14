model={
    publish=function(self)
        print(":::::: Publish form lua ")
        print(type(self)) 
        print(self.Name) 
        print("plop")
        self:AddEntryPoint("step", "main model step",function (self)
            print(":::::: model step from lua model "..self.Name)
        end)
    end,
    configure=function()
        print(":::::: Configure from lua") 
    end,
    connect=function()
        print(":::::: Connect from lua") 
    end,
}

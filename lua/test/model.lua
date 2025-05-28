model={
    publish=function(self)
        print(":::: pub 1")
        self:Publish({
            input={
                dblvect={1.0, 2.0, 3.0}
            },
            output={
                cpt=0
            }
        })
        print(":::: pub 2")
        self:AddEntryPoint("step", "main model step",function (self)
            print("::::: lua model step")
            self.cpt = self.cpt + 1
            print(self.cpt)
        end)
        print(":::: pub 3")
    end,
    configure=function(self)
        print(":::::: Configure from lua") 
    end,
    connect=function(self)
        print(":::::: Connect from lua") 
    end
}

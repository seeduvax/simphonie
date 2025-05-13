model={
    publish=function(self)
        print(":::::: Publish form lua ")
        print(type(self)) 
        print(self.Name) 
        print("plop")
    end,
    configure=function()
        print(":::::: Configure from lua") 
    end,
    connect=function()
        print(":::::: Connect from lua") 
    end,
}

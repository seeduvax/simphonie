Module|SIMPHONIE|Méthode vides|Implémentation à revoir
--|--|--|--
kern|Publication|· Unpublish<br />· GetOperations|• PublishProperty : questionning parent of property|kern|
kern|ExInvalidParameterValue||ExInvalidParameterValue : add value in message (for each type)
kern|Operation| CreateRequest : ?| DeleteRequest : check if request exist
kern|Simulator|• Abort<br />• Exit<br />• GetFactories<br />• Hold (multi-threaded)<br />• Restore : deserialize models states from file|• Reconnect : Besoin de changer l'état du simulateur?<br />• Simulator : get simulator in resolver<br />• CreateInstance : add new instance in a container when the parent is set?<br />• connect : appels récursif sur les enfant<br />• configure : recursive call on childs|
kern|TypeRegistry|• AddEnumerationType : PTK_Int64 not useable, issue with SMP interface definition|• TypeRegistry : missing type<br />• AddFloatType : Ajouter sous-classe pour stocker min, max, unit …<br />• AddIntegerType : Ajouter sous-classe pour stocker min, max, unit ...<br />• AddArrayType<br />• AddClassType : verfier classtype|
kern|PublishOperation|PublishParameter : ?|
kern|ExTypeAlreadyRegistered||ExTypeAlreadyRegistered : display uuid of type|
kern|StructureType||setup : dirty method to addFields|
kern|Request||SetReturnValue : implement Smp::InvalidReturnValue|
kern|EventManager||Subscribe : unsafe thrread method|
kern|Scheduler||schedule : questionning entrypoint change its own simulation time<br />check mutable: mutable std::mutex|
kern|LinkRegistry|ILinkingComponent à appeler pour suprimer les links|
kern|Factory||CreateInstance : Gérer les noms invalid, Smp::InvalidObjectName|
kern|TimeKeeper||time management in public should be manage by a simulation|
kern|TimeKeeper|• Store<br />• Restore|
smpdk|Uuid||check if uuid decryption is not breakable|smpdk|
smpdk|Component||Disconnect() : not complete (deallocation)|
smpdk|Object||checkName (questioning forbidden words)<br />questionning private/protected functions|
sys|ChronoTool||schedule statistics : measure execution time|sys|
sys|Logger||add enum log level|
umdl|SysTimeSynchro||nanosec not compiling, ms/1000 is used|umdl|
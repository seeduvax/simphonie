Module|SIMPHONIE|Méthode vides|Implémentation à revoir|Priorité
--|--|--|--|--
kern|Publication|· Unpublish<br />· GetOperations|• PublishProperty : questionning parent of property|4|
kern|ExInvalidParameterValue||ExInvalidParameterValue : add value in message (for each type)|1|
kern|Operation| CreateRequest : ?| DeleteRequest : check if request exist|4|
kern|Simulator|• Abort<br />• Exit<br />• GetFactories<br />• Hold (multi-threaded)<br />• Restore : deserialize models states from file|• Reconnect : Besoin de changer l'état du simulateur?<br />• configure : recursive call on childs<br />• connect : appels récursif sur les enfant<br />• CreateInstance : add new instance in a container when the parent is set? Check if it's needed to pulish/configure/connect immediately after adding component<br />• Simulator : get simulator in resolver|1|
kern|TypeRegistry|• AddEnumerationType : PTK_Int64 not useable, issue with SMP interface definition|• TypeRegistry : missing type<br />• AddFloatType : Ajouter sous-classe pour stocker min, max, unit …<br />• AddIntegerType : Ajouter sous-classe pour stocker min, max, unit ...<br />• AddArrayType<br />• AddClassType : verfier classtype|2|
kern|PublishOperation|PublishParameter||3|
kern|ExTypeAlreadyRegistered||ExTypeAlreadyRegistered : display uuid of type|3|
kern|StructureType||setup : dirty method to addFields|1|
kern|Request||SetReturnValue : implement Smp::InvalidReturnValue|3|
kern|EventManager||Subscribe : unsafe thread method|3|
kern|Scheduler||schedule : questionning entrypoint change its own simulation time<br />• CreateInstance : Gérer les noms invalid, Smp::InvalidObjectName<br />• dépendance Scheduler - TimeKeeper|2|
kern|LinkRegistry|ILinkingComponent à appeler pour supprimer les links||4|
kern|Persist|• Store<br />• Restore||4|
kern|Resolver|• GetEntryPoint appelé par  getasbolute<br />• gestion data ou tableau||1|
kern|Factory||CreateInstance : Gérer les noms invalid, Smp::InvalidObjectName|4|
kern|TimeKeeper||time management in public should be manage by a simulation|2|
smpdk|Uuid||check if uuid decryption is not breakable<br />• faciliter création à partir string|1|
smpdk|Component||Disconnect() : not complete (deallocation)|1|
smpdk|AnySimple||Opérateur ostream|1|
smpdk|Object||checkName (questioning forbidden words)<br />questionning private/protected functions|4|
sys|ChronoTool||schedule statistics : measure execution time|3|
sys|Logger||add enum log level|2|
umdl|SysTimeSynchro||nanosec not compiling, ms/1000 is used|4|
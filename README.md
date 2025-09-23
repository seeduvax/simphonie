# SimDecK: SMP (ECSS-ST-E-40-07-C) dev kit library
![alt text](_doc/src/logoSimphonie.png?raw=true "Simphonie Logo")

---

This library is part of the simphonie project, used by the project for its own
needs. However it can be used to support any SMP related application or library.

Caution: the primary git repository hosts two main "components"
  - simdeck: the SMP SDK subject of this readme and tracked with all simdeck-*
    branches.
  - simphonie: A SMP runtime implementation, tracked with all simphonie-*
    branches.

## SMP / ECSS-E-ST-40-07C
SMP stands for Simulation Modelling Platform. It is an [ECSS][0] standard 
defining C++ interface for simulation models and simulation execution
infrastructure. Simphonie is a direct and simplistic (partial) implementation
of this standard enabling its learning and experimentation. 

## Disclamer
Do not expect simdeck to be a full and validated SMP SDK. It was
created for prototyping and training purpose and shall be use for anything else
only after a strong validation of the fitness to the target use case.

This software is distributed in the hope that it well be useful, but *without
any warrenty*; without even the implied warrenty of *merchantability* or
*fitness for a particular purpose*. Simphonie is free software released under
LGPL V3. See [LICENSE][4] file for more information.

Despite simphonie itself is free software, the SMP stajndard definition is 
subject to copyright hold by the European Space Agency for the member of ECSS.
See the [ECSS policy of use][2] for more details. The SMP header files are
published using a BSD licence (referee complete information directly). And the
build process should fetch the needed file from the main package repository used
for simphonie development.

## Build
- Clone the project and checkout the branch or tag you need.
- Enter the project root directory (the one holding the `app.cfg` file)
- build the project

```
    make
```

- run tests

```
    make test

```
- build the library package

```
    make dist
```

See [AcrobatomaticBuildSystem][5] documentation for more details about the 
build features, in particular the required dependencies fetching. 

## project tools:

- [AcrobatomaticBuildSystem][5]: compilation script, packaging and dependencies 
management. Run `make help` to get the quick reference guide.

Issues and tasks are managed with ABS itself. Once cloned or export a workspace 
run, the basic issue management actions are accessible through make targets:
  - `make crls`: list issues for the current branch.
  - `make crsel <issue_id>`: select the issue to work on. This will set the
    issue reference on message of the next commit action.
  - `make crnew`: create new issue.


[0]:http://ecss.nl
[1]:https://ecss.nl/standard/ecss-e-st-40-07c-simulation-modelling-platform-2-march-2020/
[4]:LICENSE
[5]:https://github.com/seeduvax/AcrobatomaticBuildSystem

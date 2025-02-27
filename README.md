# SiMPhonie: simulation kernel implementing SMP (ECSS-ST-E-40-07-C)
![alt text](_doc/src/logoSimphonie.png?raw=true "Simphonie Logo")

---

## SMP / ECSS-E-ST-40-07C
SMP stands for Simulation Modelling Platform. It is an [ECSS][0] standard 
defining C++ interface for simulation models and simulation execution
infrastructure. Simphonie is a direct and simplistic (partial) implementation
of this standard enabling its learning and experimentation. 

## Disclamer
Do not expect simphonie to be a full and validated SMP implementation. It was
created for prototyping and training purpose and shall be use for anything else
only after a strong validation of the fitness to the target use case.

This software is distributed in the hope that it well be useful, but *without
any warrenty*; without even the implied warrenty of *merchantability* or
*fitness for a particular purpose*. Simphonie is free software released under
LGPL V3. See [LICENSE][4] file for more information.


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
- build installation package

```
    make distinstall
```

See [AcrobatomaticBuildSystem][5] documentation for more details about the 
build features, in particular the required dependencies fetch. The dependencies
package repository. This repository may not host the needed package for your
specific target architecture. You may yourself build the missing packages to be
stored in your local ABS cache storage (`~/.abs/cache/<archname>/).

The simphonie project is itself split into two packages managed from the same
git repository. Each package is subject to its own branches and tags set:
 - simdeck-X.Y: SMP models and simulation services development kit - set of
   concrete classes implementing the basic SMP interfaces needed for models and
   services development.
 - simphonie-X.Y: SMP simuulation runtime environment.  

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

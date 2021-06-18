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

Despite simphonie itself is free software, the required SMP C++ headers are
subject to copyright hold by the European Space Agency for the member of ECSS.
See the [ECSS policy of use][2] for more details. In consequence, the SMP 
headers are not included in simphonie and can't be fetched automatically when
building the software. However when you are able to get by yourself the [SMP
material][1], the makefile includes a specific target to assist in the header
integration into the project.

## Build
- Clone the project and checkout the branch or tag you need.
- Enter the project root directory (the one holding the `app.cfg` file)
- Download the SMP zip file from the [standard's introduction page][1].
- install the header
```
    make install_smpheader <zipfile>
```
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
build features. 

## project tools:

- [AcrobatomaticBuildSystem][5]: compilation script, packaging and dependencies 
management.

Issues and tasks are managed with ABS itself. Once cloned or export a workspace 
run `make crls` from the project root directory to get the issues list 
related to the current branch.

[0]:http://ecss.nl
[1]:https://ecss.nl/standard/ecss-e-st-40-07c-simulation-modelling-platform-2-march-2020/
[2]:https://ecss.nl/standards/license-agreement-disclaimer/
[4]:LICENSE
[5]:https://github.com/seeduvax/AcrobatomaticBuildSystem

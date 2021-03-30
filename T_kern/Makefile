# Generated make bootstrap, do not edit. Edit module.cfg to configure module build.
include ../Makefile

# TODO target 'shell' as an abs extension ?

## Targets
##  - shell [RUNARGS="<arg> [arg]*"]: run lua shell using application 
##      environment (mastol needed in USELIB)
LUAD?=lua
.PHONY: shell
shell: all
	@LD_LIBRARY_PATH=$(LDLIBP) $(LUAD) $(RUNARGS)

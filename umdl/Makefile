# Generated make bootstrap, do not edit. Edit module.cfg to configure module build.
include ../Makefile

# TODO target 'shell' as an abs extension ?

LUA_CPATH:="$(subst :,/?.so;,$(LDLIBP))/?.so"

## Targets
##  - shell [RUNARGS="<arg> [arg]*"]: run lua shell using application 
##      environment (mastol needed in USELIB)
LUAD?=lua
.PHONY: shell
shell: all
	@LUA_CPATH=$(LUA_CPATH) LD_LIBRARY_PATH=$(LDLIBP) $(LUAD) $(RUNARGS)

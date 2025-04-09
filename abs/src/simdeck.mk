ifeq ($(__SIMDECK_MK),)
__SIMDECK_MK:=defined

ifneq ($(filter CentOS_7_%,$(ARCH)),)

EcssSmpDir=$(_app_ecss.smp_dir)/include/Smp
UuidHFile=$(EcssSmpDir)/Uuid.h
uuidhpatchedFile=$(PRJOBJDIR)/.ecss.smp.patched

# patch the file Uuid.h that doing a forward declaration that is not working on centos7/clang13
$(uuidhpatchedFile):
	@$(ABS_PRINT_info) "Patching file $(UuidHFile)"
	@chmod +w $(UuidHFile)
	@chmod +w $(EcssSmpDir)
	@sed -i 's/template<typename T> struct hash;//g' $(UuidHFile)
	@chmod -w $(UuidHFile)
	@chmod -w $(EcssSmpDir)
	@touch $@

$(PRJOBJDIR)/$(MODNAME)/.depready: $(uuidhpatchedFile)

endif

endif

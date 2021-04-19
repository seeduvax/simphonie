ifeq ($(__GIT_MK),)
__GIT_MK:=defined

ifneq ($(USER),$(JENKINS_USER))
ifneq ($(wildcard $(PRJROOT)/.git/hooks),)
# enabled along with the commit message hook,
# it installs a pre-commit hook to check coding style format 
# It applies a coding style with clang-format 
# only to new code added and only with developper agreement
$(PRJROOT)/.git/hooks/pre-commit: $(PRJROOT)/.pre-commit
	@$(ABS_PRINT_info) "Installing pre-commit git hook"
	@mkdir -p $(@D)
	@cp $^ $@
	@chmod +x $@

$(PRJROOT)/.git/hooks/apply-format: $(PRJROOT)/.apply-format
	@$(ABS_PRINT_info) "Installing coding style apply-format script"
	@mkdir -p $(@D)
	@cp $^ $@
	@chmod +x $@

## - check-format: check git staged contents coding style format. display diff changes to make
.PHONY: check-format
check-format:
	@cd $(PRJROOT); .git/hooks/apply-format --staged

## - format: format locally your git staged contents you made and that you are about to commit
.PHONY: format
format:
	@cd $(PRJROOT); .git/hooks/apply-format --staged -i && git status

## - format-cached: format git staged contents you made and that you are about to commit
.PHONY: format-cached
format-cached:
	@cd $(PRJROOT); .git/hooks/apply-format --cached -i && git status

# insert the hook as an early dependency to trigger its installation as soon as possible
$(PRJROOT)/app.cfg: $(PRJROOT)/.git/hooks/pre-commit $(PRJROOT)/.git/hooks/apply-format

endif
endif
endif

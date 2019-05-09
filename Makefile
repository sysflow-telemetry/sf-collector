include makefile.manifest.inc
include makefile.env.inc

.PHONY: all
all: modules sysporter

.PHONY: modules
modules:
	make -C modules

.PHONY: sysporter
sysporter:
	make -C src

.PHONY: install
install: 
	make -C modules install
	make -C src install

.PHONY: uninstall
uninstall: 
	make -C src uninstall
	make -C modules uninstall

.PHONY: clean
clean:
	make -C src clean
	make -C modules clean 

.PHONY : help
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... modules"
	@echo "... sysporter"
	@echo "... install"
	@echo "... uninstall"

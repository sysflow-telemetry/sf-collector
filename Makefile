include makefile.manifest.inc
include makefile.env.inc

.PHONY: all
all: modules sysporter

.PHONY: modules
modules:
	cd modules && make

.PHONY: sysporter
sysporter:
	cd src && make

.PHONY: install
install: 
	cd modules && make install
	cd src && make install

.PHONY: uninstall
uninstall: 
	cd src && make uninstall
	cd modules && make uninstall

.PHONY: clean
clean:
	cd src && make clean
	cd modules && make clean 

.PHONY : help
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... modules"
	@echo "... sysporter"
	@echo "... install"
	@echo "... uninstall"

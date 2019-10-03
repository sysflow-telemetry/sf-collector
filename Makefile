#
# Copyright (C) 2019 IBM Corporation.
#
# Authors:
# Frederico Araujo <frederico.araujo@ibm.com>
# Teryl Taylor <terylt@ibm.com>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

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

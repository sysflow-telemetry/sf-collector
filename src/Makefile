#!/bin/bash
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

# Build environment configuration
include ../makefile.manifest.inc
include ../makefile.env.inc

# Target configuration
TARGET = sysporter
TARGET2 = sysreader

# Lint options
LINT = clang-tidy-9
LINTCHECKS = "bugprone-*"
#LINTCHECKS = "-abseil*,-android*,bugprone-*,cert-*,clang-*,cppcoreguidelines-*,google-*,hicpp-*,\
	     -llvm-header-guard,misc-*,modernize-*,-modernize-use-trailing-return-type,-modernize-loop-convert,\
	     -modernize-make-unique,-modernize-pass-by-value, performance-*,-readability-*,\
	     -readability-convert-member-functions-to-static"
#LINTSRCS := $(filter-out MurmurHash3.cpp, $(wildcard *.cpp))
LINTSRCS := main.cpp
LINTHEADERS = "^($(shell pwd)\/)((?!logger).)*"

# Dir structure configuration
LIBLOCALPREFIX = ../modules
SDLOCALLIBPREFIX = $(LIBLOCALPREFIX)/sysdig/build/lib
SDLOCALINCPREFIX = $(LIBLOCALPREFIX)/sysdig/build/include
AVRLOCALLIBPREFIX = $(LIBLOCALPREFIX)/avro/lang/c++/build
AVRLOCALINCPREFIX = $(LIBLOCALPREFIX)/avro/lang/c++/build

# Compiler options 
CXX = g++
LIBS = $(SDLOCALLIBPREFIX)/libsinsp.a $(SDLOCALLIBPREFIX)/libscap.a $(SDLOCALLIBPREFIX)/libjq.a \
      $(SDLOCALLIBPREFIX)/libb64.a $(SDLOCALLIBPREFIX)/libcurl.a $(SDLOCALLIBPREFIX)/libtbb.a \
      -lstdc++ -lelf -lz -lrt -lanl -lssl -lcrypto -lpthread -lm -lavrocpp -llog4cxx
LDFLAGS = $(LIBS) -L/usr/local/lib/ -L$(LIBPREFIX)/ -L$(SDLOCALLIBPREFIX)/ -L$(AVRLOCALLIBPREFIX)/ \
	 -Wl,-rpath=/usr/local/lib -Wl,-rpath=$(LIBPREFIX) -Wl,-rpath=$(SDLOCALLIBPREFIX) -Wl,-rpath=$(AVRLOCALLIBPREFIX)
CFLAGS = -std=c++11 -Wall -ggdb -I.. -I../modules/sysflow/c++/ -I../modules/filesystem/include/ \
		-DHAS_CAPTURE -DPLATFORM_NAME=\"Linux\" -DK8S_DISABLE_THREAD \
	 	-I$(SDLOCALINCPREFIX)/ -I$(SDLOCALINCPREFIX)/curl/ -I$(SDLOCALINCPREFIX)/json/ -I$(SDLOCALINCPREFIX)/openssl/ -I$(SDLOCALINCPREFIX)/driver/ \
		-I$(SDLOCALINCPREFIX)/userspace/libsinsp/ -I$(SDLOCALINCPREFIX)/userspace/libscap/ -I$(AVRLOCALINCPREFIX)/ -I/usr/local/include/

.PHONY: all
all: version $(TARGET)
#all: version $(TARGET) $(TARGET2)

.PHONY: install
install: all 
	mkdir -p $(PREFIX)/bin && cp sysporter $(PREFIX)/bin && cp sysreader $(PREFIX)/bin
	mkdir -p $(PREFIX)/conf && cp avro/avsc/SysFlow.avsc $(PREFIX)/conf && cp conf/log4cxx.properties $(PREFIX)/conf

.PHONY: uninstall
uninstall: 
	rm -rf $(PREFIX)/bin
	rm -rf $(PREFIX)/conf

.PHONY: lint
lint: $(LINTSRCS)
	$(LINT) -checks=$(LINTCHECKS) -header-filter=$(LINTHEADERS) $^ -- $(CFLAGS)

.PHONY: lintfix
lintfix: $(LINTSRCS)
	$(LINT) -checks=$(LINTCHECKS) -format-style=llvm -header-filter=$(LINTHEADERS) -fix $^ -- $(CFLAGS)

.PHONY: version
version:
	cp sysflow_config.h.in sysflow_config.h
	sed -i -E "s/SYSFLOW_VERSION/\"$(SYSFLOW_VERSION)\"/" sysflow_config.h
	sed -i -E "s/SYSFLOW_BUILD_NUMBER/\"$(SYSFLOW_BUILD_NUMBER)\"/" sysflow_config.h

.PHONY: $(TARGET)
$(TARGET): .main.o .MurmurHash3.o .utils.o .containercontext.o .processcontext.o .processeventprocessor.o .dataflowprocessor.o .networkflowprocessor.o .fileflowprocessor.o .fileeventprocessor.o .sysflowcontext.o .sysflowprocessor.o .sysflowwriter.o .filecontext.o
	$(CXX) $^ -o $@ $(LDFLAGS)

.PHONY: $(TARGET2)
$(TARGET2): .reader.o .MurmurHash3.o
	$(CXX) $^ -o $@ $(LDFLAGS)

.main.o: main.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.sysflowfork.o: sysflowfork.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.MurmurHash3.o: MurmurHash3.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.utils.o: utils.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.containercontext.o: containercontext.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.sysflowprocessor.o: sysflowprocessor.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.sysflowwriter.o: sysflowwriter.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.processcontext.o: processcontext.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.processeventprocessor.o: processeventprocessor.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.dataflowprocessor.o: dataflowprocessor.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.networkflowprocessor.o: networkflowprocessor.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.networkflowprocessor.lint: networkflowprocessor.cpp
	$(LINT) -checks=* $^ -- $(CFLAGS)

.fileflowprocessor.o: fileflowprocessor.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.fileeventprocessor.o: fileeventprocessor.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.sysflowcontext.o: sysflowcontext.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.filecontext.o: filecontext.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.reader.o: reader.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.PHONY: clean
clean:
	rm -f .[!.]*.o *.o *.so *.a $(TARGET) $(TARGET2) sysflow_config.h

.PHONY : help
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... install"
	@echo "... uninstall"

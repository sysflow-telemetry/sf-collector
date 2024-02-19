#!/bin/bash
#
# Copyright (C) 2024 IBM Corporation.
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
include ../../makefile.manifest.inc
include ../../makefile.env.inc

# Target configuration
TARGET = libsysflow.a 
SYSFLOW_BUILD_NUMBER ?= 0
BUNDLE_DEPS ?= 1

# Lint options
LINT = clang-tidy-9
LINTCHECKS = "bugprone-*"
# To enable more checks, uncomment below
#LINTCHECKS = "-abseil*,-android*,bugprone-*,cert-*,clang-*,cppcoreguidelines-*,google-*,hicpp-*,\
	     -llvm-header-guard,misc-*,modernize-*,-modernize-use-trailing-return-type,-modernize-loop-convert,\
	     -modernize-make-unique,-modernize-pass-by-value, performance-*,-readability-*,\
	     -readability-convert-member-functions-to-static"
LINTSRCS := $(wildcard *.cpp)
LINTHEADERS = "^($(shell pwd)\/)((?!logger).)*"

# Dir structure configuration
MODPREFIX ?= ../../modules
FALCOINCPREFIX ?= $(MODPREFIX)/falco-libs/build/include
AVRINCPREFIX ?= $(MODPREFIX)/avro/lang/c++/build
SFINCPREFIX ?= $(MODPREFIX)/sysflow/c++
FSINCPREFIX ?= $(MODPREFIX)/filesystem/include
SCHPREFIX ?= $(MODPREFIX)/sysflow/avro/avsc
ELF_RPATH ?= /usr/lib/sysflow
DEBUG ?= 0
ASAN ?= 0
MUSL ?= 0
ARCH ?= x86_64

# Compiler options
CXX = g++
AR = ar
MUSLFLAGS = -static -Os 
CFLAGS = -std=c++17 -Wall -I.. -I/usr/local/include/ -I/usr/include/ \
		 -DHAS_CAPTURE -DPLATFORM_NAME=\"Linux\" -DK8S_DISABLE_THREAD \
		 -I$(SFINCPREFIX)/ \
		 -I$(FSINCPREFIX)/ \
	 	 -I$(FALCOINCPREFIX)/ \
		 -I$(FALCOINCPREFIX)/curl/ \
		 -I$(FALCOINCPREFIX)/json2/ \
		 -I$(FALCOINCPREFIX)/openssl/ \
		 -I$(FALCOINCPREFIX)/driver/ \
		 -I$(FALCOINCPREFIX)/userspace/libsinsp/ \
		 -I$(FALCOINCPREFIX)/userspace/libscap/ \
		 -I$(FALCOINCPREFIX)/userspace/common/ \
		 -I$(AVRINCPREFIX)/

OBJS = .sysflowlibs.o .sysflowlibs.o .MurmurHash3.o .utils.o .containercontext.o .processcontext.o .processeventprocessor.o .controlflowprocessor.o .dataflowprocessor.o .networkflowprocessor.o .fileflowprocessor.o .fileeventprocessor.o .sysflowcontext.o .sysflowprocessor.o .sysflowwriter.o .sffilewriter.o .sfsockwriter.o .sfmultiwriter.o .sfcallbackwriter.o .filecontext.o .k8scontext.o .k8seventprocessor.o .modutils.o .sysflowexception.o

$(info    MUSL is $(MUSL))
ifeq ($(MUSL), 1)
	CFLAGS += $(MUSLFLAGS) -DHAVE_STRLCPY
endif

$(info    DEBUG is $(DEBUG))
ifeq ($(DEBUG), 1)
	CFLAGS += -ggdb
else
	CFLAGS += -O3
endif

$(info    ASAN is $(ASAN))
ifeq ($(ASAN), 1)
	CFLAGS += -fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -fsanitize=leak
endif

.PHONY: all
all: version $(TARGET)

.PHONY: install
install: all
	mkdir -p $(INSTALL_PATH)/lib && cp libsysflow.a $(INSTALL_PATH)/lib
	mkdir -p $(INSTALL_PATH)/include/sysflow/ && cp *.h $(INSTALL_PATH)/include/sysflow/ && cp *.hpp $(INSTALL_PATH)/include/sysflow/
ifeq ($(BUNDLE_DEPS), 1)
ifeq ($(MUSL), 0)
	$(AR) -M <libsysflow.mri
else
	$(AR) -M <libsysflow-musl.mri
	mkdir -p $(ELF_RPATH) && \
	cp `readlink -f /lib/ld-musl-$(ARCH).so.1` $(ELF_RPATH)/ld-musl-$(ARCH).so.1 && \
	cp `readlink -f /usr/lib/libstdc++.so.6` $(ELF_RPATH)/libstdc++.so.6 && \
	cp `readlink -f /usr/lib/libelf.so.1` $(ELF_RPATH)/libelf.so.1 && \
	cp `readlink -f /usr/lib/libelf.so.1` $(ELF_RPATH)/libelf.so && \
	cp `readlink -f /usr/lib/libgcc_s.so.1` $(ELF_RPATH)/libgcc_s.so.1 && \
	cp `readlink -f /lib/libc.musl-$(ARCH).so.1` $(ELF_RPATH)/libc.musl-$(ARCH).so.1 && \
	cp `readlink -f /lib/libz.so.1` $(ELF_RPATH)/libz.so.1 && \
	cp `readlink -f /usr/lib/libzstd.so.1` $(ELF_RPATH)/libzstd.so.1
endif
	cp libsysflow_with_deps.a $(INSTALL_PATH)/lib
endif

.PHONY: uninstall
uninstall:
	rm -rf $(INSTALL_PATH)/lib $(ELF_RPATH)

.PHONY: lint
lint: $(LINTSRCS)
	$(LINT) -checks=$(LINTCHECKS) -header-filter=$(LINTHEADERS) $^ -- $(CFLAGS)

.PHONY: lintfix
lintfix: $(LINTSRCS)
	$(LINT) -checks=$(LINTCHECKS) -format-style=llvm -header-filter=$(LINTHEADERS) -fix $^ -- $(CFLAGS)

version:
	cp sysflow_config.h.in sysflow_config.h
	sed -i -E "s/SYSFLOW_VERSION/\"$(SYSFLOW_VERSION)\"/" sysflow_config.h
	sed -i -E "s/SYSFLOW_BUILD_NUMBER/\"$(SYSFLOW_BUILD_NUMBER)\"/" sysflow_config.h

.PHONY: $(TARGET)
$(TARGET): $(OBJS) 
	$(AR) -r $@ $^

.sysflowlibs.o: sysflowlibs.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.test.o: test.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.sysflowfork.o: sysflowfork.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.MurmurHash3.o: MurmurHash3.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.utils.o: utils.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.k8scontext.o: k8scontext.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.k8seventprocessor.o: k8seventprocessor.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.containercontext.o: containercontext.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.sysflowprocessor.o: sysflowprocessor.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.sysflowwriter.o: sysflowwriter.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.sffilewriter.o: sffilewriter.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.sfsockwriter.o: sfsockwriter.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.sfcallbackwriter.o: sfcallbackwriter.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.sfmultiwriter.o: sfmultiwriter.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.processcontext.o: processcontext.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.processeventprocessor.o: processeventprocessor.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.controlflowprocessor.o: controlflowprocessor.cpp
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

.modutils.o: modutils.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.sysflowexception.o: sysflowexception.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.PHONY: clean
clean:
	rm -f .[!.]*.o *.o *.so *.a $(TARGET) 

.PHONY : help
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... install"
	@echo "... uninstall"

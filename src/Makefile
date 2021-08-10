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
SYSFLOW_BUILD_NUMBER ?= 0

# Lint options
LINT = clang-tidy-9
LINTCHECKS = "bugprone-*"
# To enable more checks, uncomment below
#LINTCHECKS = "-abseil*,-android*,bugprone-*,cert-*,clang-*,cppcoreguidelines-*,google-*,hicpp-*,\
	     -llvm-header-guard,misc-*,modernize-*,-modernize-use-trailing-return-type,-modernize-loop-convert,\
	     -modernize-make-unique,-modernize-pass-by-value, performance-*,-readability-*,\
	     -readability-convert-member-functions-to-static"
LINTSRCS := $(filter-out MurmurHash3.cpp, $(wildcard *.cpp))
LINTHEADERS = "^($(shell pwd)\/)((?!logger).)*"

# Dir structure configuration
LIBLOCALPREFIX ?= ../modules
SDLOCALLIBPREFIX ?= $(LIBLOCALPREFIX)/sysdig/build/lib
SDLOCALINCPREFIX ?= $(LIBLOCALPREFIX)/sysdig/build/include
AVRLOCALLIBPREFIX ?= $(LIBLOCALPREFIX)/avro/lang/c++/build
AVRLOCALINCPREFIX ?= $(LIBLOCALPREFIX)/avro/lang/c++/build
SFLOCALINCPREFIX ?= $(LIBLOCALPREFIX)/sysflow/c++
FSLOCALINCPREFIX ?= $(LIBLOCALPREFIX)/filesystem/include
SCHLOCALPREFIX ?= $(LIBLOCALPREFIX)/sysflow/avro/avsc
DEBUG ?= 0


# Compiler options 
CXX = g++
LIBS = $(SDLOCALLIBPREFIX)/libsinsp.a $(SDLOCALLIBPREFIX)/libscap.a $(SDLOCALLIBPREFIX)/libjq.a $(SDLOCALLIBPREFIX)/libonig.a \
      $(SDLOCALLIBPREFIX)/libb64.a $(SDLOCALLIBPREFIX)/libcurl.a $(SDLOCALLIBPREFIX)/libtbb.a $(SDLOCALLIBPREFIX)/libgrpc++.a $(SDLOCALLIBPREFIX)/libgrpc++_alts.a $(SDLOCALLIBPREFIX)/libgrpc++_reflection.a  $(SDLOCALLIBPREFIX)/libgrpc++_error_details.a  $(SDLOCALLIBPREFIX)/libgrpc++_unsecure.a  $(SDLOCALLIBPREFIX)/libgrpc_plugin_support.a $(SDLOCALLIBPREFIX)/libgrpcpp_channelz.a   $(SDLOCALLIBPREFIX)/libgrpc.a $(SDLOCALLIBPREFIX)/libgpr.a -lcares -lprotobuf $(SDLOCALLIBPREFIX)/abseil-cpp/absl/synchronization/libabsl_synchronization.a $(SDLOCALLIBPREFIX)/abseil-cpp/absl/synchronization/libabsl_graphcycles_internal.a  $(SDLOCALLIBPREFIX)/abseil-cpp/absl/status/libabsl_status.a $(SDLOCALLIBPREFIX)/abseil-cpp/absl/strings/libabsl_strings.a $(SDLOCALLIBPREFIX)/abseil-cpp/absl/strings/libabsl_strings_internal.a $(SDLOCALLIBPREFIX)/abseil-cpp/absl/strings/libabsl_cord.a  $(SDLOCALLIBPREFIX)/abseil-cpp/absl/strings/libabsl_str_format_internal.a  $(SDLOCALLIBPREFIX)/abseil-cpp/absl/base/libabsl_raw_logging_internal.a $(SDLOCALLIBPREFIX)/abseil-cpp/absl/base/libabsl_malloc_internal.a $(SDLOCALLIBPREFIX)/abseil-cpp/absl/base/libabsl_scoped_set_env.a $(SDLOCALLIBPREFIX)/abseil-cpp/absl/base/libabsl_base.a $(SDLOCALLIBPREFIX)/abseil-cpp/absl/base/libabsl_spinlock_wait.a $(SDLOCALLIBPREFIX)/abseil-cpp/absl/base/libabsl_throw_delegate.a $(SDLOCALLIBPREFIX)/abseil-cpp/absl/base/libabsl_log_severity.a $(SDLOCALLIBPREFIX)/abseil-cpp/absl/types/libabsl_bad_optional_access.a $(SDLOCALLIBPREFIX)/abseil-cpp/absl/time/libabsl_time.a $(SDLOCALLIBPREFIX)/abseil-cpp/absl/time/libabsl_time_zone.a $(SDLOCALLIBPREFIX)/abseil-cpp/absl/numeric/libabsl_int128.a $(SDLOCALLIBPREFIX)/abseil-cpp/absl/debugging/libabsl_stacktrace.a $(SDLOCALLIBPREFIX)/abseil-cpp/absl/debugging/libabsl_symbolize.a $(SDLOCALLIBPREFIX)/abseil-cpp/absl/debugging/libabsl_debugging_internal.a $(SDLOCALLIBPREFIX)/abseil-cpp/absl/debugging/libabsl_demangle_internal.a  $(SDLOCALLIBPREFIX)/abseil-cpp/absl/base/libabsl_dynamic_annotations.a \
      -lstdc++ -lelf -lz -lrt -lanl -lssl -lcrypto -lpthread -lm -lavrocpp -lglog -ldl -lupb -laddress_sorting -lre2
LDFLAGS = $(LIBS) -L/usr/local/lib/ -L$(LIBPREFIX)/ -L$(SDLOCALLIBPREFIX)/ -L$(AVRLOCALLIBPREFIX)/ \
	 -Wl,-rpath=/usr/local/lib -Wl,-rpath=$(LIBPREFIX) -Wl,-rpath=$(SDLOCALLIBPREFIX) -Wl,-rpath=$(AVRLOCALLIBPREFIX)
CFLAGS = -std=c++11 -Wall -I.. -I$(SFLOCALINCPREFIX)/ -I$(FSLOCALINCPREFIX)/ \
		-DHAS_CAPTURE -DPLATFORM_NAME=\"Linux\" \
	 	-I$(SDLOCALINCPREFIX)/ -I$(SDLOCALINCPREFIX)/curl/ -I$(SDLOCALINCPREFIX)/json/ -I$(SDLOCALINCPREFIX)/openssl/ -I$(SDLOCALINCPREFIX)/driver/ \
		-I$(SDLOCALINCPREFIX)/userspace/libsinsp/ -I$(SDLOCALINCPREFIX)/userspace/libscap/ -I$(AVRLOCALINCPREFIX)/ -I/usr/local/include/ -I/usr/include/

$(info    DEBUG is $(DEBUG))
ifeq ($(DEBUG), 1)
	CFLAGS += -ggdb
	LIBS += -lprofiler -ltcmalloc
else
	CFLAGS += -O3
endif

.PHONY: all
all: version $(TARGET)

.PHONY: install
install: all 
	mkdir -p $(INSTALL_PATH)/bin && cp sysporter $(INSTALL_PATH)/bin
	mkdir -p $(INSTALL_PATH)/conf && cp $(SCHLOCALPREFIX)/SysFlow.avsc $(INSTALL_PATH)/conf

.PHONY: uninstall
uninstall: 
	rm -rf $(INSTALL_PATH)/bin
	rm -rf $(INSTALL_PATH)/conf

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
$(TARGET): .main.o .MurmurHash3.o .utils.o .k8scontext.o .containercontext.o .processcontext.o .k8seventprocessor.o .processeventprocessor.o .controlflowprocessor.o .dataflowprocessor.o .networkflowprocessor.o .fileflowprocessor.o .fileeventprocessor.o .sysflowcontext.o .sysflowprocessor.o .sysflowwriter.o .sffilewriter.o .sfsockwriter.o .sfmultiwriter.o .filecontext.o
	$(CXX) $^ -o $@ $(LDFLAGS)

.main.o: main.cpp
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

#.xxhash.o: xxhash.c
#	$(CXX) $(CFLAGS) -o $@ -c $^

.PHONY: clean
clean:
	rm -f .[!.]*.o *.o *.so *.a $(TARGET) sysflow_config.h

.PHONY : help
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... install"
	@echo "... uninstall"

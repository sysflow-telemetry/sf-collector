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
include ../../makefile.manifest.inc
include ../../makefile.env.inc

# Target configuration
TARGET = libsysflow.a 
TARGET_TEST = test
SYSFLOW_BUILD_NUMBER ?= 0
LIB_TARGET = libsfcollector.a
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
LIBLOCALPREFIX ?= ../../modules
SDLOCALLIBPREFIX ?= $(LIBLOCALPREFIX)/falco-libs/build/lib
SDLOCALINCPREFIX ?= $(LIBLOCALPREFIX)/falco-libs/build/include
AVRLOCALLIBPREFIX ?= $(LIBLOCALPREFIX)/avro/lang/c++/build
AVRLOCALINCPREFIX ?= $(LIBLOCALPREFIX)/avro/lang/c++/build
SFLOCALINCPREFIX ?= $(LIBLOCALPREFIX)/sysflow/c++
FSLOCALINCPREFIX ?= $(LIBLOCALPREFIX)/filesystem/include
SCHLOCALPREFIX ?= $(LIBLOCALPREFIX)/sysflow/avro/avsc
FALCOSECURITY_LIBS_LIB_PREFIX = $(SDLOCALLIBPREFIX)
DEBUG ?= 0

# Compiler options
CXX = g++
AR = ar
LIBS = $(TARGET) $(FALCOSECURITY_LIBS_LIB_PREFIX)/libsinsp.a $(FALCOSECURITY_LIBS_LIB_PREFIX)/libscap.a $(FALCOSECURITY_LIBS_LIB_PREFIX)/libjq.a $(FALCOSECURITY_LIBS_LIB_PREFIX)/libonig.a \
       $(FALCOSECURITY_LIBS_LIB_PREFIX)/libb64.a $(FALCOSECURITY_LIBS_LIB_PREFIX)/libcurl.a $(FALCOSECURITY_LIBS_LIB_PREFIX)/libtbb.a $(FALCOSECURITY_LIBS_LIB_PREFIX)/libgrpc++.a \
       $(FALCOSECURITY_LIBS_LIB_PREFIX)/libgrpc++_alts.a $(FALCOSECURITY_LIBS_LIB_PREFIX)/libgrpc++_reflection.a  $(FALCOSECURITY_LIBS_LIB_PREFIX)/libgrpc++_error_details.a \
       $(FALCOSECURITY_LIBS_LIB_PREFIX)/libgrpc++_unsecure.a  $(FALCOSECURITY_LIBS_LIB_PREFIX)/libgrpc_plugin_support.a $(FALCOSECURITY_LIBS_LIB_PREFIX)/libgrpcpp_channelz.a \
       $(FALCOSECURITY_LIBS_LIB_PREFIX)/libgrpc.a $(FALCOSECURITY_LIBS_LIB_PREFIX)/libgpr.a -lcares -lprotobuf  \
       $(FALCOSECURITY_LIBS_LIB_PREFIX)/libsinsp.a $(FALCOSECURITY_LIBS_LIB_PREFIX)/libscap.a $(FALCOSECURITY_LIBS_LIB_PREFIX)/libjq.a $(FALCOSECURITY_LIBS_LIB_PREFIX)/libonig.a \
       $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_synchronization.a $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_graphcycles_internal.a  $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_status.a \
       $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_statusor.a $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_strings.a $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_strings_internal.a \
       $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_cord.a $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_cordz_functions.a $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_cord_internal.a \
       $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_cordz_info.a $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_cordz_handle.a $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_hash.a \
       $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_city.a $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_low_level_hash.a $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_raw_hash_set.a \
       $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_hashtablez_sampler.a  $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_random_internal_pool_urbg.a \
       $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_random_internal_randen.a $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_random_internal_randen_hwaes.a \
       $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_random_internal_randen_hwaes_impl.a $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_random_internal_platform.a \
       $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_random_internal_randen_slow.a $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_random_internal_seed_material.a \
       $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_random_seed_gen_exception.a $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_exponential_biased.a \
       $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_bad_variant_access.a $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_str_format_internal.a  \
       $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_raw_logging_internal.a $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_malloc_internal.a $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_base.a \
       $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_spinlock_wait.a $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_throw_delegate.a $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_log_severity.a \
       $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_bad_optional_access.a $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_time.a $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_time_zone.a \
       $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_int128.a $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_stacktrace.a $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_symbolize.a \
       $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_debugging_internal.a $(FALCOSECURITY_LIBS_LIB_PREFIX)/libabsl_demangle_internal.a \
       $(AVRLOCALLIBPREFIX)/libavrocpp_s.a \
	   $(shell find /usr/lib* -name "libboost_iostreams.a") \
	   $(shell find /usr/local* -name "libsnappy.a") \
	   $(shell find /usr/local* -name "libglog.a") \
	   $(shell find /usr/local* -name "libelf.a") \
	  -lstdc++ -lz -lrt -lanl -lssl -lcrypto -lpthread -lm -ldl -lupb -laddress_sorting -lre2
LDFLAGS = $(LIBS) -L$(LIBPREFIX)/ -L$(SDLOCALLIBPREFIX)/ -L$(AVRLOCALLIBPREFIX)/ #-L/usr/lib/x86_64-linux-gnu -Wl,-rpath=/usr/lib/x86_64-linux-gnu
CFLAGS = -std=c++11 -Wall -I.. -I$(SFLOCALINCPREFIX)/ -I$(FSLOCALINCPREFIX)/ \
		-DHAS_CAPTURE -DPLATFORM_NAME=\"Linux\" -DK8S_DISABLE_THREAD \
	 	-I$(SDLOCALINCPREFIX)/ -I$(SDLOCALINCPREFIX)/curl/ -I$(SDLOCALINCPREFIX)/json2/ -I$(SDLOCALINCPREFIX)/openssl/ -I$(SDLOCALINCPREFIX)/driver/ \
		-I$(SDLOCALINCPREFIX)/userspace/libsinsp/ -I$(SDLOCALINCPREFIX)/userspace/libscap/ -I$(AVRLOCALINCPREFIX)/ -I/usr/local/include/ -I/usr/include/

OBJS = .sysflowlibs.o .sysflowlibs.o .MurmurHash3.o .utils.o .containercontext.o .processcontext.o .processeventprocessor.o .controlflowprocessor.o .dataflowprocessor.o .networkflowprocessor.o .fileflowprocessor.o .fileeventprocessor.o .sysflowcontext.o .sysflowprocessor.o .sysflowwriter.o .sffilewriter.o .sfsockwriter.o .sfmultiwriter.o .sfcallbackwriter.o .filecontext.o .k8scontext.o .k8seventprocessor.o

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
	mkdir -p $(INSTALL_PATH)/lib && cp libsysflow.a $(INSTALL_PATH)/lib
	mkdir -p $(INSTALL_PATH)/include/sysflow/ && cp *.h $(INSTALL_PATH)/include/sysflow/ && cp *.hpp $(INSTALL_PATH)/include/sysflow/

.PHONY: uninstall
uninstall:
	rm -rf $(INSTALL_PATH)/lib

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

.PHONY: $(TARGET_TEST)
$(TARGET_TEST): $(OBJS) .test.o 
	$(CXX) $^ -o $@ $(LDFLAGS)

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

#.xxhash.o: xxhash.c
#	$(CXX) $(CFLAGS) -o $@ -c $^

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

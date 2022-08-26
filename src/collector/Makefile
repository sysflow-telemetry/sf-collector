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

# Dir structure configuration (these are overridden during docker build) 
LIBLOCALPREFIX ?= ../../modules
FALCOLOCALLIBPREFIX ?= $(LIBLOCALPREFIX)/falco-libs/build/lib
FALCOLOCALINCPREFIX ?= $(LIBLOCALPREFIX)/falco-libs/build/include
AVRLOCALLIBPREFIX ?= $(LIBLOCALPREFIX)/avro/lang/c++/build
AVRLOCALINCPREFIX ?= $(LIBLOCALPREFIX)/avro/lang/c++/build
SFLOCALINCPREFIX ?= $(LIBLOCALPREFIX)/sysflow/c++
SFLOCALLIBPREFIX ?= $(LIBLOCALPREFIX)/lib
FSLOCALINCPREFIX ?= $(LIBLOCALPREFIX)/filesystem/include
SCHLOCALPREFIX ?= $(LIBLOCALPREFIX)/sysflow/avro/avsc
DEBUG ?= 0
ASAN ?= 0
MUSL ?= 0

# Compiler options
CXX = g++
LIBS = ../libs/libsysflow.a \
	   $(FALCOLOCALLIBPREFIX)/libsinsp.a \
       $(FALCOLOCALLIBPREFIX)/libscap.a $(FALCOLOCALLIBPREFIX)/libscap_engine_bpf.a $(FALCOLOCALLIBPREFIX)/libscap_engine_kmod.a \
	   /build/modules/falco-libs/build/libscap/libdriver_event_schema.a \
	   $(FALCOLOCALLIBPREFIX)/libscap_engine_noop.a $(FALCOLOCALLIBPREFIX)/libscap_engine_source_plugin.a $(FALCOLOCALLIBPREFIX)/libscap_engine_gvisor.a \
	   $(FALCOLOCALLIBPREFIX)/libscap_engine_nodriver.a $(FALCOLOCALLIBPREFIX)/libscap_engine_udig.a $(FALCOLOCALLIBPREFIX)/libscap_event_schema.a \
       $(FALCOLOCALLIBPREFIX)/libscap_engine_util.a $(FALCOLOCALLIBPREFIX)/libjq.a $(FALCOLOCALLIBPREFIX)/libonig.a \
	   /build/modules/falco-libs/build/libscap/engine/savefile/libscap_engine_savefile.a \
	   /build/modules/falco-libs/build/libscap/engine/kmod/libscap_engine_kmod.a \
	   /build/modules/falco-libs/build/libscap/engine/bpf/libscap_engine_bpf.a \
	   $(FALCOLOCALLIBPREFIX)/libb64.a $(FALCOLOCALLIBPREFIX)/libcurl.a $(FALCOLOCALLIBPREFIX)/libtbb.a $(FALCOLOCALLIBPREFIX)/libgrpc++.a \
       $(FALCOLOCALLIBPREFIX)/libgrpc++_alts.a $(FALCOLOCALLIBPREFIX)/libgrpc++_reflection.a  $(FALCOLOCALLIBPREFIX)/libgrpc++_error_details.a \
       $(FALCOLOCALLIBPREFIX)/libgrpc++_unsecure.a  $(FALCOLOCALLIBPREFIX)/libgrpc_plugin_support.a $(FALCOLOCALLIBPREFIX)/libgrpcpp_channelz.a \
       $(FALCOLOCALLIBPREFIX)/libgrpc.a $(FALCOLOCALLIBPREFIX)/libgpr.a $(FALCOLOCALLIBPREFIX)/libabsl_synchronization.a \
	   $(FALCOLOCALLIBPREFIX)/libabsl_graphcycles_internal.a  $(FALCOLOCALLIBPREFIX)/libabsl_status.a $(FALCOLOCALLIBPREFIX)/libabsl_statusor.a \
	   $(FALCOLOCALLIBPREFIX)/libabsl_strings.a $(FALCOLOCALLIBPREFIX)/libabsl_strings_internal.a $(FALCOLOCALLIBPREFIX)/libabsl_cord.a \
	   $(FALCOLOCALLIBPREFIX)/libabsl_cordz_functions.a $(FALCOLOCALLIBPREFIX)/libabsl_cord_internal.a \
	   $(FALCOLOCALLIBPREFIX)/libabsl_cordz_info.a $(FALCOLOCALLIBPREFIX)/libabsl_cordz_handle.a $(FALCOLOCALLIBPREFIX)/libabsl_hash.a \
	   $(FALCOLOCALLIBPREFIX)/libabsl_city.a $(FALCOLOCALLIBPREFIX)/libabsl_low_level_hash.a $(FALCOLOCALLIBPREFIX)/libabsl_raw_hash_set.a \
	   $(FALCOLOCALLIBPREFIX)/libabsl_hashtablez_sampler.a $(FALCOLOCALLIBPREFIX)/libabsl_random_internal_pool_urbg.a \
       $(FALCOLOCALLIBPREFIX)/libabsl_random_internal_randen.a $(FALCOLOCALLIBPREFIX)/libabsl_random_internal_randen_hwaes.a \
       $(FALCOLOCALLIBPREFIX)/libabsl_random_internal_randen_hwaes_impl.a $(FALCOLOCALLIBPREFIX)/libabsl_random_internal_platform.a \
       $(FALCOLOCALLIBPREFIX)/libabsl_random_internal_randen_slow.a $(FALCOLOCALLIBPREFIX)/libabsl_random_internal_seed_material.a \
       $(FALCOLOCALLIBPREFIX)/libabsl_random_seed_gen_exception.a $(FALCOLOCALLIBPREFIX)/libabsl_exponential_biased.a \
       $(FALCOLOCALLIBPREFIX)/libabsl_bad_variant_access.a $(FALCOLOCALLIBPREFIX)/libabsl_str_format_internal.a  \
       $(FALCOLOCALLIBPREFIX)/libabsl_raw_logging_internal.a $(FALCOLOCALLIBPREFIX)/libabsl_malloc_internal.a $(FALCOLOCALLIBPREFIX)/libabsl_base.a \
       $(FALCOLOCALLIBPREFIX)/libabsl_spinlock_wait.a $(FALCOLOCALLIBPREFIX)/libabsl_throw_delegate.a $(FALCOLOCALLIBPREFIX)/libabsl_log_severity.a \
       $(FALCOLOCALLIBPREFIX)/libabsl_bad_optional_access.a $(FALCOLOCALLIBPREFIX)/libabsl_time.a $(FALCOLOCALLIBPREFIX)/libabsl_time_zone.a \
       $(FALCOLOCALLIBPREFIX)/libabsl_int128.a $(FALCOLOCALLIBPREFIX)/libabsl_stacktrace.a $(FALCOLOCALLIBPREFIX)/libabsl_symbolize.a \
       $(FALCOLOCALLIBPREFIX)/libabsl_debugging_internal.a $(FALCOLOCALLIBPREFIX)/libabsl_demangle_internal.a \
	   $(AVRLOCALLIBPREFIX)/libavrocpp_s.a \
	   $(shell find /usr/lib* -name "libboost_iostreams.a") \
	   $(shell find /usr/local* -name "libsnappy.a") \
	   $(shell find /usr/local* -name "libglog.a") \
	   $(shell find /usr/lib* -name "libelf.a") \
	   -lstdc++ -lz -lssl -lcrypto -lpthread -lm -ldl -lupb -laddress_sorting -lre2 -lcares -lprotobuf
MUSLFLAGS = -static -Os
LDFLAGS = $(LIBS) -L$(LIBPREFIX)/ -L$(FALCOLOCALLIBPREFIX)/ -L$(AVRLOCALLIBPREFIX)/ 
CFLAGS = -std=c++11 -Wall -I.. -I$(SFLOCALINCPREFIX)/ -I../libs/ -I$(FSLOCALINCPREFIX)/ \
		-DHAS_CAPTURE -DPLATFORM_NAME=\"Linux\" -DK8S_DISABLE_THREAD \
	 	-I$(FALCOLOCALINCPREFIX)/ -I$(FALCOLOCALINCPREFIX)/curl/ -I$(FALCOLOCALINCPREFIX)/json2/ -I$(FALCOLOCALINCPREFIX)/openssl/ -I$(FALCOLOCALINCPREFIX)/driver/ \
		-I$(FALCOLOCALINCPREFIX)/userspace/libsinsp/ -I$(FALCOLOCALINCPREFIX)/userspace/libscap/ -I$(AVRLOCALINCPREFIX)/ -I/usr/local/include/ -I/usr/include/ 

$(info    MUSL is $(MUSL))
ifeq ($(MUSL), 1)
	CFLAGS += $(MUSLFLAGS) -fPIE -pie -DHAVE_STRLCPY
	LDFLAGS += $(MUSLFLAGS)
else 
	LIBS += -lrt -lanl
endif

$(info    DEBUG is $(DEBUG))
ifeq ($(DEBUG), 1)
	CFLAGS += -ggdb
	LIBS += -lprofiler -ltcmalloc
else
	CFLAGS += -O3
endif

$(info    ASAN is $(ASAN))
ifeq ($(ASAN), 1)
	CFLAGS += -fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -fsanitize=leak
	LIBS += -fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -fsanitize=leak
endif

.PHONY: all
all: $(TARGET) $(LIB_TARGET)

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

.PHONY: $(TARGET)
$(TARGET): .main.o
	$(CXX) $^ -o $@ $(LDFLAGS)

.main.o: main.cpp
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

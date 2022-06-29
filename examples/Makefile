#!/bin/bash
#
# Copyright (C) 2022 IBM Corporation.
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

# Target configuration
TARGET = callback
INSTALL_PATH ?= /usr/local/example
SYSFLOW_VERSION ?= 0.5.0-libsysflow-alpha.1

# Dir structure configuration
SYSFLOW_LIB_PREFIX = /usr/lib/sysflow
SYSFLOW_INCLUDE_PREFIX = /usr/include/sysflow
FALCO_LIBS_LIB_PREFIX = /usr/lib/falcosecurity
FALCO_LIBS_INCLUDE_PREFIX = /usr/include/falcosecurity

# Compiler options
DEBUG ?= 0
CXX = g++
LIBS = $(FALCO_LIBS_LIB_PREFIX)/libsinsp.a $(FALCO_LIBS_LIB_PREFIX)/libscap.a $(FALCO_LIBS_LIB_PREFIX)/libjq.a $(FALCO_LIBS_LIB_PREFIX)/libonig.a \
       $(FALCO_LIBS_LIB_PREFIX)/libb64.a $(FALCO_LIBS_LIB_PREFIX)/libcurl.a $(FALCO_LIBS_LIB_PREFIX)/libtbb.a $(FALCO_LIBS_LIB_PREFIX)/libgrpc++.a \
       $(FALCO_LIBS_LIB_PREFIX)/libgrpc++_alts.a $(FALCO_LIBS_LIB_PREFIX)/libgrpc++_reflection.a  $(FALCO_LIBS_LIB_PREFIX)/libgrpc++_error_details.a \
       $(FALCO_LIBS_LIB_PREFIX)/libgrpc++_unsecure.a  $(FALCO_LIBS_LIB_PREFIX)/libgrpc_plugin_support.a $(FALCO_LIBS_LIB_PREFIX)/libgrpcpp_channelz.a \
       $(FALCO_LIBS_LIB_PREFIX)/libgrpc.a $(FALCO_LIBS_LIB_PREFIX)/libgpr.a  \
       $(FALCO_LIBS_LIB_PREFIX)/libsinsp.a $(FALCO_LIBS_LIB_PREFIX)/libscap.a $(FALCO_LIBS_LIB_PREFIX)/libjq.a $(FALCO_LIBS_LIB_PREFIX)/libonig.a \
       $(FALCO_LIBS_LIB_PREFIX)/libabsl_synchronization.a $(FALCO_LIBS_LIB_PREFIX)/libabsl_graphcycles_internal.a  $(FALCO_LIBS_LIB_PREFIX)/libabsl_status.a \
       $(FALCO_LIBS_LIB_PREFIX)/libabsl_statusor.a $(FALCO_LIBS_LIB_PREFIX)/libabsl_strings.a $(FALCO_LIBS_LIB_PREFIX)/libabsl_strings_internal.a \
       $(FALCO_LIBS_LIB_PREFIX)/libabsl_cord.a $(FALCO_LIBS_LIB_PREFIX)/libabsl_cordz_functions.a $(FALCO_LIBS_LIB_PREFIX)/libabsl_cord_internal.a \
       $(FALCO_LIBS_LIB_PREFIX)/libabsl_cordz_info.a $(FALCO_LIBS_LIB_PREFIX)/libabsl_cordz_handle.a $(FALCO_LIBS_LIB_PREFIX)/libabsl_hash.a \
       $(FALCO_LIBS_LIB_PREFIX)/libabsl_city.a $(FALCO_LIBS_LIB_PREFIX)/libabsl_low_level_hash.a $(FALCO_LIBS_LIB_PREFIX)/libabsl_raw_hash_set.a \
       $(FALCO_LIBS_LIB_PREFIX)/libabsl_hashtablez_sampler.a  $(FALCO_LIBS_LIB_PREFIX)/libabsl_random_internal_pool_urbg.a \
       $(FALCO_LIBS_LIB_PREFIX)/libabsl_random_internal_randen.a $(FALCO_LIBS_LIB_PREFIX)/libabsl_random_internal_randen_hwaes.a \
       $(FALCO_LIBS_LIB_PREFIX)/libabsl_random_internal_randen_hwaes_impl.a $(FALCO_LIBS_LIB_PREFIX)/libabsl_random_internal_platform.a \
       $(FALCO_LIBS_LIB_PREFIX)/libabsl_random_internal_randen_slow.a $(FALCO_LIBS_LIB_PREFIX)/libabsl_random_internal_seed_material.a \
       $(FALCO_LIBS_LIB_PREFIX)/libabsl_random_seed_gen_exception.a $(FALCO_LIBS_LIB_PREFIX)/libabsl_exponential_biased.a \
       $(FALCO_LIBS_LIB_PREFIX)/libabsl_bad_variant_access.a $(FALCO_LIBS_LIB_PREFIX)/libabsl_str_format_internal.a  \
       $(FALCO_LIBS_LIB_PREFIX)/libabsl_raw_logging_internal.a $(FALCO_LIBS_LIB_PREFIX)/libabsl_malloc_internal.a $(FALCO_LIBS_LIB_PREFIX)/libabsl_base.a \
       $(FALCO_LIBS_LIB_PREFIX)/libabsl_spinlock_wait.a $(FALCO_LIBS_LIB_PREFIX)/libabsl_throw_delegate.a $(FALCO_LIBS_LIB_PREFIX)/libabsl_log_severity.a \
       $(FALCO_LIBS_LIB_PREFIX)/libabsl_bad_optional_access.a $(FALCO_LIBS_LIB_PREFIX)/libabsl_time.a $(FALCO_LIBS_LIB_PREFIX)/libabsl_time_zone.a \
       $(FALCO_LIBS_LIB_PREFIX)/libabsl_int128.a $(FALCO_LIBS_LIB_PREFIX)/libabsl_stacktrace.a $(FALCO_LIBS_LIB_PREFIX)/libabsl_symbolize.a \
       $(FALCO_LIBS_LIB_PREFIX)/libabsl_debugging_internal.a $(FALCO_LIBS_LIB_PREFIX)/libabsl_demangle_internal.a \
       $(SYSFLOW_LIB_PREFIX)/libsysflow.a $(SYSFLOW_LIB_PREFIX)/libavrocpp_s.a $(SYSFLOW_LIB_PREFIX)/libsnappy.a $(SYSFLOW_LIB_PREFIX)/libglog.a $(SYSFLOW_LIB_PREFIX)/libelf.a \
       $(shell find /usr/lib* -name "libboost_iostreams.a") \
      -lstdc++ -lz -lrt -lanl -lssl -lcrypto -lpthread -lm -ldl -lupb -laddress_sorting -lre2 -lcares -lprotobuf
LDFLAGS = $(LIBS) -L$(FALCO_LIBS_LIB_PREFIX) -L$(SYSFLOW_LIB_PREFIX)
CFLAGS = -std=c++11 -Wall -DHAS_CAPTURE -DPLATFORM_NAME=\"Linux\" -DK8S_DISABLE_THREAD \
                -I$(SYSFLOW_INCLUDE_PREFIX)/ -I$(SYSFLOW_INCLUDE_PREFIX)/filesystem/ -I$(SYSFLOW_INCLUDE_PREFIX)/mods/ \
                -I$(FALCO_LIBS_INCLUDE_PREFIX)/ -I$(FALCO_LIBS_INCLUDE_PREFIX)/curl/ -I$(FALCO_LIBS_INCLUDE_PREFIX)/json2/ -I$(FALCO_LIBS_INCLUDE_PREFIX)/openssl/ -I$(FALCO_LIBS_INCLUDE_PREFIX)/driver/ \
                -I$(FALCO_LIBS_INCLUDE_PREFIX)/userspace/libsinsp/ -I$(FALCO_LIBS_INCLUDE_PREFIX)/userspace/libscap/ -I$(FALCO_LIBS_INCLUDE_PREFIX)/avro -I/usr/local/include/ -I/usr/include/

$(info    DEBUG is $(DEBUG))
ifeq ($(DEBUG), 1)
        CFLAGS += -ggdb
        LIBS += -lprofiler -ltcmalloc
else
        CFLAGS += -O3
endif

.PHONY: all
all: $(TARGET)

.PHONY: install
install: all
	mkdir -p $(INSTALL_PATH)/bin && cp $(TARGET) $(INSTALL_PATH)/bin

.PHONY: uninstall
uninstall:
	rm -rf $(INSTALL_PATH)/bin/$(TARGET)

.PHONY: docker-build
docker-build:
	( DOCKER_BUILDKIT=1 docker build --build-arg SYSFLOW_VERSION=${SYSFLOW_VERSION} -t callback:${SYSFLOW_VERSION} -f Dockerfile . )
	docker run --rm -v $(shell pwd):/mnt --entrypoint=/bin/bash callback:${SYSFLOW_VERSION} -c "cp /usr/local/example/bin/callback /mnt/."

.PHONY: $(TARGET)
$(TARGET): callback.o
	$(CXX) $^ -o $@ $(LDFLAGS)

callback.o: callback.cpp
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

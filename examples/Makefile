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
include manifest
TARGET = callback
INSTALL_PATH ?= /usr/local/example

# Dir structure configuration
SYSFLOW_LIB_PREFIX = /usr/lib/sysflow
SYSFLOW_INCLUDE_PREFIX = /usr/include/sysflow
FALCO_LIBS_LIB_PREFIX = /usr/lib/falcosecurity
FALCO_LIBS_INCLUDE_PREFIX = /usr/include/falcosecurity

# Compiler options
DEBUG ?= 0
MUSL ?= 0
CLANG_FORMAT = clang-format-12
CXX = g++
LIBS = $(SYSFLOW_LIB_PREFIX)/libsysflow_with_deps.a \
      -lstdc++ -lz -lssl -lcrypto -lpthread -lm -ldl -lupb -laddress_sorting -lre2 -lcares -lprotobuf
MUSLFLAGS = -static -Os
LDFLAGS = $(LIBS) -L$(FALCO_LIBS_LIB_PREFIX) -L$(SYSFLOW_LIB_PREFIX)
CFLAGS = -std=c++11 -Wall -DHAS_CAPTURE -DPLATFORM_NAME=\"Linux\" -DK8S_DISABLE_THREAD \
                -I$(SYSFLOW_INCLUDE_PREFIX)/ -I$(SYSFLOW_INCLUDE_PREFIX)/filesystem/ -I$(SYSFLOW_INCLUDE_PREFIX)/mods/ \
                -I$(FALCO_LIBS_INCLUDE_PREFIX)/ -I$(FALCO_LIBS_INCLUDE_PREFIX)/curl/ -I$(FALCO_LIBS_INCLUDE_PREFIX)/json2/ -I$(FALCO_LIBS_INCLUDE_PREFIX)/openssl/ -I$(FALCO_LIBS_INCLUDE_PREFIX)/driver/ \
                -I$(FALCO_LIBS_INCLUDE_PREFIX)/userspace/libsinsp/ -I$(FALCO_LIBS_INCLUDE_PREFIX)/userspace/libscap/ -I$(FALCO_LIBS_INCLUDE_PREFIX)/avro -I/usr/local/include/ -I/usr/include/

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

.PHONY: all
all: $(TARGET)

.PHONY: install
install: all
	mkdir -p $(INSTALL_PATH)/bin && cp $(TARGET) $(INSTALL_PATH)/bin

.PHONY: uninstall
uninstall:
	rm -rf $(INSTALL_PATH)/bin/$(TARGET)

.PHONY: build
build:
	( DOCKER_BUILDKIT=1 docker build --build-arg SYSFLOW_VERSION=${SYSFLOW_VERSION} --build-arg FALCO_LIBS_VER=${FALCO_LIBS_VERSION} --build-arg FALCO_VER=${FALCO_VERSION} --build-arg UBI_VER=${UBI_VERSION} -t callback:${SYSFLOW_VERSION} -f Dockerfile . )
	docker run --rm -v $(shell pwd):/mnt --entrypoint=/bin/bash callback:${SYSFLOW_VERSION} -c "cp /usr/local/example/bin/callback /mnt/."

.PHONY: build/dev
build/dev:
	( DOCKER_BUILDKIT=1 docker build --build-arg SYSFLOW_VERSION=${SYSFLOW_VERSION} --build-arg FALCO_LIBS_VER=${FALCO_LIBS_VERSION} --build-arg FALCO_VER=${FALCO_VERSION} --build-arg UBI_VER=${UBI_VERSION} -t callback:${SYSFLOW_VERSION} -f Dockerfile.dev . )
	docker run --rm -v $(shell pwd):/mnt --entrypoint=/bin/bash callback:${SYSFLOW_VERSION} -c "cp /usr/local/example/bin/callback /mnt/."

.PHONY: build/musl
build/musl:
	( DOCKER_BUILDKIT=1 docker build --build-arg SYSFLOW_VERSION=${SYSFLOW_VERSION} --build-arg FALCO_LIBS_VER=${FALCO_LIBS_VERSION} --build-arg FALCO_VER=${FALCO_VERSION} --build-arg UBI_VER=${UBI_VERSION} -t callback:${SYSFLOW_VERSION} -f Dockerfile.musl . )
	docker run --rm -v $(shell pwd):/mnt --entrypoint=/bin/bash callback:${SYSFLOW_VERSION} -c "cp /usr/local/example/bin/callback /mnt/."

.PHONY: build/musldev
build/musldev:
	( DOCKER_BUILDKIT=1 docker build --build-arg SYSFLOW_VERSION=${SYSFLOW_VERSION} --build-arg FALCO_LIBS_VER=${FALCO_LIBS_VERSION} --build-arg FALCO_VER=${FALCO_VERSION} --build-arg UBI_VER=${UBI_VERSION} -t callback:${SYSFLOW_VERSION} -f Dockerfile.musl.dev . )
	docker run --rm -v $(shell pwd):/mnt --entrypoint=/bin/bash callback:${SYSFLOW_VERSION} -c "cp /usr/local/example/bin/callback /mnt/."

.PHONY: format
format:
	$(CLANG_FORMAT) -i --style=file *.cpp

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
	@echo "... build"
	@echo "... build/musl"
	@echo "... build/dev"
	@echo "... build/musldev"
	@echo "... format"

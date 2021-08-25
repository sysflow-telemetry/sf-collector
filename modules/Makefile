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

include ../makefile.manifest.inc
include ../makefile.env.inc

.PHONY: all
all: init \
	 modules

.PHONY: init
init:
	git submodule update --init --recursive --force
	cd falco && git checkout $(FALCO_VERSION)
	cd falco-libs && git checkout $(FALCO_LIBS_VERSION)
	cd avro && git checkout $(AVRO_VERSION)

.PHONY: modules
modules: falcolibs/package \
         avro/package

.PHONY: install
install: modules \
	falcolibs/install \
	avro/install \
	sysflow/install \
	fs/install

.PHONY: falcolibs/package
falcolibs/package: 
	cd falco-libs && sed -i -E "s/SYSDIG_VERSION \".+\"/SYSDIG_VERSION \"$(FALCO_LIBS_VERSION)\"/" CMakeLists.txt && \
	patch -p1 -i ../falco/cmake/modules/falcosecurity-libs-repo/patch/libscap.patch && mkdir -p build && cd build && \
        cmake -DPROBE_NAME=falco ../. && make tbb grpc c-ares zlib jq curl protobuf b64 openssl scap sinsp && \
	mkdir -p include && mkdir -p lib && mkdir -p bin && \
	mkdir -p include/curl && cp curl-prefix/src/curl/include/curl/*.h include/curl && \
	mkdir -p include/driver && cp driver/src/*h include/driver && cp ../driver/*h include/driver && \
	mkdir -p include/json && cp ../userspace/libsinsp/third-party/jsoncpp/jsoncpp.cpp include/json && \
	mkdir -p include/json && cp ../userspace/libsinsp/third-party/jsoncpp/json/*.h include/json && \
	mkdir -p include/userspace/common && cp ../userspace/common/*.h include/userspace/common && \
	mkdir -p include/userspace/libsinsp && cp ../userspace/libsinsp/*.h include/userspace/libsinsp && \
	mkdir -p include/userspace/libsinsp/container_engine && cp ../userspace/libsinsp/container_engine/*.h include/userspace/libsinsp/container_engine/ && \
	mkdir -p include/userspace/libsinsp/include && cp ../userspace/libsinsp/include/*.h include/userspace/libsinsp/include/ && \
	mkdir -p include/userspace/libscap && cp ../userspace/libscap/*.h include/userspace/libscap && \
	mkdir -p include/openssl && cp openssl-prefix/src/openssl/include/openssl/*.h include/openssl && \
	cp -r tbb-prefix/src/tbb/include/serial  include/ && \
	cp -r tbb-prefix/src/tbb/include/tbb include/ && \
	cp -r protobuf-prefix/src/protobuf/src/google include/ && \
	cp tbb-prefix/src/tbb/build/lib_release/libtbb.a lib/ && \
	cp tbb-prefix/src/tbb/build/lib_release/libtbbmalloc.a lib/ && \
	cp -r c-ares-prefix/src/c-ares/target/include/*.h include/ && \
	cp -r b64-prefix/src/b64/include/b64 include/ && \
	cp -r jq-prefix/src/jq/target/include/*.h include/ && \
	cp libsinsp/libsinsp.a lib/ && \
	cp libscap/libscap.a lib/ && \
	cp -r ../driver/bpf driver/src/ && \
	cp ../../falco/docker/falco/docker-entrypoint.sh bin/ && \
	cp ../../falco/scripts/falco-driver-loader bin/ && \
	sed -i -E "s/@PROBE_VERSION@/\"$(FALCO_LIBS_VERSION)\"/" bin/falco-driver-loader && \
	sed -i -E "s/@FALCO_VERSION@/\"$(FALCO_VERSION)\"/" bin/falco-driver-loader && \
	sed -i -E "s#@DRIVERS_REPO@#\"$(DRIVERS_REPO)\"#" bin/falco-driver-loader && \
	cp grpc-prefix/src/grpc/*.a lib/ && \
	cp ./grpc-prefix/src/grpc/third_party/re2/libre2.a lib/ && \
	cp -r grpc-prefix/src/grpc/third_party/abseil-cpp lib/ && \
	cp protobuf-prefix/src/protobuf/src/.libs/* lib/ && \
	cp c-ares-prefix/src/c-ares/.libs/* lib/ && \
	cp b64-prefix/src/b64/src/libb64.a lib/ && \
	cp curl-prefix/src/curl/lib/.libs/libcurl.a lib/ && \
	cp jq-prefix/src/jq/target/lib/*.a lib/ && \
	cp tbb-prefix/src/tbb/build/lib_release/libtbb.a lib/ && \
        cp openssl-prefix/src/openssl/*.a lib/ && \
	cp protobuf-prefix/src/protobuf/src/.libs/* lib && \
	cp zlib-prefix/src/zlib/libz.so.1.2.11 lib/libz.so

.PHONY: falcolibs/install
falcolibs/install:	
	mkdir -p $(SDINCPREFIX) && mkdir -p $(LIBPREFIX) && mkdir -p $(BINPREFIX) && mkdir -p "$(DRIVERPREFIX)$(FALCO_LIBS_VERSION)" && \
	cd falco-libs/build && cp -r include/* $(SDINCPREFIX)/ && cp -a lib/* $(LIBPREFIX)/ && cp -a bin/* $(BINPREFIX)/ && cp -a driver/src/* "$(DRIVERPREFIX)$(FALCO_LIBS_VERSION)"

.PHONY: avro/package
avro/package:
	cd avro/lang/c++ && mkdir -p build && cd build && \
	cmake -DCMAKE_INSTALL_PREFIX=$(MODPREFIX) \
	-DCMAKE_CXX_FLAGS_ALL_WARNINGS:STRING="-w" \
	-DCMAKE_BUILD_TYPE=ALL_WARNINGS -G "Unix Makefiles" ../. && make && \
	cp -a ../api avro

.PHONY: avro/install
avro/install:
	cd avro/lang/c++/build && make install

.PHONY: sysflow/install
sysflow/install:
	mkdir -p $(CONFPREFIX) && cp sysflow/avro/avsc/SysFlow.avsc $(CONFPREFIX)/ && \
	mkdir -p $(SFINCPREFIX)/c++/sysflow && cp -r sysflow/c++/sysflow/* $(SFINCPREFIX)/c++/sysflow/

.PHONY: fs/install
fs/install:
	mkdir -p $(FSINCPREFIX) && cp -r filesystem/include/* $(FSINCPREFIX)/

.PHONY: uninstall
uninstall:
	rm -rf $(LIBPREFIX) $(INCPREFIX) $(CONFPREFIX) && \
	rm -rf $(DRIVERPREFIX)$(FALCO_LIBS_VERSION) && \
	find $(BINPREFIX) ! -name "sysporter" -type f -exec rm -f {} +

.PHONY: clean
clean:
	rm -rf avro/lang/c++/build falco/libs/build

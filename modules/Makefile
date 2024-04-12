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

MAKE_JOBS ?= 1
BUNDLE_FALCO_LIBS ?= 1

$(info    MUSL is $(MUSL))
ifeq ($(MUSL), 1)
	MUSL_FLAG = -DMUSL_OPTIMIZED_BUILD=ON
endif

.PHONY: all
all: init \
	 modules

.PHONY: init
init:
	git submodule update --init --recursive --force
	cd falco && git fetch origin && git checkout $(FALCO_VERSION)
ifneq ($(FALCO_LIBS_PR),)
	cd falco-libs && git fetch origin pull/$(FALCO_LIBS_PR)/head:pull_$(FALCO_LIBS_PR) && git checkout pull_$(FALCO_LIBS_PR)
else
	cd falco-libs && git fetch origin && git checkout $(FALCO_LIBS_VERSION)
endif
	cd falco-driver && git fetch origin && git checkout $(FALCO_LIBS_DRIVER_VERSION)
	cd avro && git fetch origin && git checkout $(AVRO_VERSION)
	cd elf && git fetch origin && git checkout $(ELF_VERSION)
	cd glog && git fetch origin && git checkout $(GLOG_VERSION)
	cd snappy && git fetch origin && git checkout $(SNAPPY_VERSION)
	cd dkms && git fetch origin && git checkout $(DKMS_VERSION)
	cd filesystem && git fetch origin && git checkout $(GHCFS_VERSION)
	cd sparsehash && git fetch origin && git checkout $(SPARSE_VERSION)
	cd xxHash && git fetch origin && git checkout $(XXHASH_VERSION)

# libelf is a pre-requisite for libs build
.PHONY: modules
modules: elf/package \
	    elf/install \
		falcolibs/package \
		falcodriver/package \
		avro/package \
		glog/package \
		snappy/package \
		xxhash/package \
		sparsehash/package

.PHONY: install
install: modules \
	falcolibs/install \
	falcodriver/install \
	avro/install \
	sysflow/install \
	fs/install \
	glog/install \
	snappy/install \
	dkms-rh/install \
	xxhash/install \
	sparsehash/install

.PHONY: falcolibs/package
falcolibs/package:
	cd falco-libs && \
	mkdir -p build && cd build && \
        cmake \
			-DDRIVER_VERSION=${FALCO_LIBS_DRIVER_VERSION} \
			-DDRIVER_PACKAGE_NAME=falco \
			-DSCAP_BPF_PROBE_ENV_VAR_NAME=FALCO_BPF_PROBE \
			-DCAP_HOST_ROOT_ENV_VAR_NAME=HOST_ROOT \
			-DFALCOSECURITY_LIBS_VERSION=${FALCO_LIBS_VERSION} \
			-DFALCOSECURITY_LIBS_CHECKSUM=${FALCO_LIBS_CHECKSUM} \
			-DCMAKE_BUILD_TYPE=Release \
			-DDRIVER_NAME=falco \
			-DBUILD_BPF=OFF \
			-DBUILD_DRIVER=OFF \
			-DBUILD_LIBSCAP_MODERN_BPF=ON \
			-DUSE_BUNDLED_GTEST=ON \
			-DUSE_BUNDLED_DEPS=ON \
			-DUSE_BUNDLED_LIBELF=OFF \
			-DCREATE_TEST_TARGETS=OFF \
			-DBUILD_LIBSCAP_EXAMPLES=OFF \
			-DBUILD_LIBSINSP_EXAMPLES=OFF \
			-DCMD_MAKE="make -j${MAKE_JOBS}" \
			${MUSL_FLAG} ../. && make -j${MAKE_JOBS} && \
	mkdir -p include && mkdir -p lib && mkdir -p bin && \
	mkdir -p include/curl && cp curl-prefix/src/curl/include/curl/*.h include/curl && \
	mkdir -p include/driver && cp driver/src/*h include/driver && cp ../driver/*h include/driver && \
	mkdir -p include/userspace/common && cp ../userspace/common/*.h include/userspace/common && cp ../build/common/*.h include/userspace/common &&\
	mkdir -p include/userspace/libsinsp && cp ../userspace/libsinsp/*.h include/userspace/libsinsp && \
	mkdir -p include/userspace/libsinsp/container_engine && cp ../userspace/libsinsp/container_engine/*.h include/userspace/libsinsp/container_engine/ && \
	mkdir -p include/userspace/libsinsp/filter && cp ../userspace/libsinsp/filter/*.h include/userspace/libsinsp/filter/ && \
	mkdir -p include/userspace/libsinsp/events && cp ../userspace/libsinsp/events/*.h include/userspace/libsinsp/events/ && \
	mkdir -p include/userspace/libsinsp/include && cp ../userspace/libsinsp/include/*.h include/userspace/libsinsp/include/ && \
	mkdir -p include/userspace/libsinsp/state && cp ../userspace/libsinsp/state/*.h include/userspace/libsinsp/state/ && \
	mkdir -p include/userspace/plugin && cp ../userspace/plugin/*.h include/userspace/plugin/ && \
	mkdir -p include/userspace/libscap && cd .. && find userspace/libscap -name '*.h' -exec cp -r --parents '{}' build/include \; && cd build && \
	mkdir -p include/openssl && cp openssl-prefix/src/openssl/include/openssl/*.h include/openssl && \
	cp -r tbb-prefix/src/tbb/include/tbb include/ && \
	cp -r tbb-prefix/src/tbb/include/oneapi include/ && \
	cp -r protobuf-prefix/src/protobuf/target/include/google include/ && \
	cp libbpf-prefix/src/libbpf-build/build/*.a lib/ && \
	cp protobuf-prefix/src/protobuf/target/lib/libprotobuf.a lib && \
	cp protobuf-prefix/src/protobuf/target/lib/libprotoc.a lib && \
	cp tbb-prefix/src/tbb/lib_release/libtbb.a lib/ && \
	cp jsoncpp-prefix/src/lib/libjsoncpp.a lib/ && \
	cp -r jsoncpp-prefix/src/include/json  include/ && \
	cp -r c-ares-prefix/src/c-ares/target/include/*.h include/ && \
	cp b64/base64.h include/ && \
	cp -r jq-prefix/src/jq/target/include/*.h include/ && \
	cp libpman/*.a lib/ && \
	cp libsinsp/*.a lib/ && \
	find libscap -name '*.a' -exec cp '{}' lib \; && \
	cp ../../falco/docker/falco/docker-entrypoint.sh bin/ && \
	cp ../../falco/scripts/falco-driver-loader bin/ && \
	sed -i -E "s/@DRIVER_VERSION@/\"$(FALCO_LIBS_DRIVER_VERSION)\"/" bin/falco-driver-loader && \
	sed -i -E "s/@FALCO_VERSION@/\"$(FALCO_VERSION)\"/" bin/falco-driver-loader && \
	sed -i -E "s#@DRIVERS_REPO@#\"$(DRIVERS_REPO)\"#" bin/falco-driver-loader && \
    cp re2-prefix/build/libre2.a lib/ && \
	cp grpc-prefix/src/grpc/*.a lib/ && \
	find grpc-prefix/src/grpc/third_party/abseil-cpp -name '*.a' -exec cp '{}' lib \; && \
	cp c-ares-prefix/src/c-ares/target/lib/libcares.a lib/ && \
	cp curl-prefix/src/curl/lib/.libs/libcurl.a lib/ && \
	cp jq-prefix/src/jq/target/lib/*.a lib/ && \
	cp openssl-prefix/src/openssl/target/lib/*.a lib/ && \
	cp zlib-prefix/src/zlib/libz.a lib/libz.a
ifeq ($(BUNDLE_FALCO_LIBS), 1)	
	cp -r falco-libs/build/lib falco-libs/build/bundle
	cp falco-libs.$(ARCH).mri falco-libs/build/bundle 
	cd falco-libs/build/bundle && \
	mv libgrpc++.a libgrpcpp.a && \
	mv libgrpc++_alts.a libgrpcpp_alts.a && \
	mv libgrpc++_error_details.a libgrpcpp_error_details.a && \
	mv libgrpc++_reflection.a libgrpcpp_reflection.a && \
	mv libgrpc++_unsecure.a libgrpcpp_unsecure.a && \
	ar -M <falco-libs.$(ARCH).mri && cp libs.a ../lib/
endif

.PHONY: falcolibs/install
falcolibs/install:
	mkdir -p $(FALCOINCPREFIX) && mkdir -p $(FALCOLIBPREFIX) && mkdir -p $(BINPREFIX) && \
	cd falco-libs/build && cp -r include/* $(FALCOINCPREFIX)/ && cp -a lib/* $(FALCOLIBPREFIX)/ && cp -a bin/* $(BINPREFIX)/

.PHONY: falcodriver/package
falcodriver/package:
	cd falco-driver && \
	mkdir -p build && cd build && \
        cmake \
			-DDRIVER_PACKAGE_NAME=falco \
			-DDRIVER_NAME=falco \
			-DCMAKE_BUILD_TYPE=Release \
			-DUSE_BUNDLED_DEPS=ON \
			-DBUILD_LIBSCAP_GVISOR=Off \
			-DBUILD_BPF=True .. && \
	cp -r ../driver/bpf driver/src/

.PHONY: falcodriver/install
falcodriver/install:
	mkdir -p "$(DRIVERPREFIX)$(FALCO_LIBS_DRIVER_VERSION)" && \
	cd falco-driver/build && cp -a driver/src/* "$(DRIVERPREFIX)$(FALCO_LIBS_DRIVER_VERSION)"

.PHONY: avro/package
avro/package:
	cd avro/lang/c++ && mkdir -p build && cd build && \
	cmake -DCMAKE_INSTALL_PREFIX=$(MODPREFIX) \
	-DCMAKE_CXX_FLAGS_ALL_WARNINGS:STRING="-w" \
	-DCMAKE_BUILD_TYPE=ALL_WARNINGS -G "Unix Makefiles" ../. && make -j${MAKE_JOBS} && \
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

.PHONY: elf/package
elf/package:
	cd elf && PREFIX=/usr make -j${MAKE_JOBS}

.PHONY: elf/install
elf/install:
	cd elf && PREFIX=/usr make -j${MAKE_JOBS} install

.PHONY: glog/package
glog/package:
	cd glog && cmake -S . -B build -G "Unix Makefiles" -DBUILD_SHARED_LIBS=OFF -DBUILD_TESTING=OFF -DWITH_GFLAGS=OFF && \
	cmake --build build -- -j${MAKE_JOBS}

.PHONY: glog/install
glog/install:
	cd glog/build && make install

.PHONY: snappy/package
snappy/package:
	cd snappy && cmake -S . -B build -G "Unix Makefiles" -DSNAPPY_BUILD_BENCHMARKS=OFF -DSNAPPY_BUILD_TESTS=OFF -DBUILD_SHARED_LIBS=OFF && \
	cmake --build build -- -j${MAKE_JOBS}

.PHONY: snappy/install
snappy/install:
	cd snappy/build && make install

.PHONY: xxhash/package
xxhash/package:
	cd xxHash && make all

.PHONY: xxhash/install
xxhash/install:
	cd xxHash && cp libxxhash.a $(LIBPREFIX) && cp libxxhash.so* $(LIBPREFIX) && cp *.h $(MODPREFIX)/include/

.PHONY: sparsehash/package
sparsehash/package:
	cd sparsehash && ./configure && make

.PHONY: sparsehash/install
sparsehash/install:
	cd sparsehash && make install

.PHONY: dkms-deb/install
dkms-deb/install:
	cd dkms && make install-debian

.PHONY: dkms-rh/install
dkms-rh/install:
	cd dkms && make install-redhat

.PHONY: uninstall
uninstall:
	rm -rf $(LIBPREFIX) $(INCPREFIX) $(CONFPREFIX) && \
	rm -rf $(DRIVERPREFIX)$(FALCO_LIBS_DRIVER_VERSION) && \
	find $(BINPREFIX) ! -name "sysporter" -type f -exec rm -f {} +

.PHONY: clean
clean:
	rm -rf avro/lang/c++/build falco-libs/build falco-driver/build glog/build snappy/build

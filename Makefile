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

MAKE_JOBS ?= 1

ARCH ?= x86_64
RUNTIME_IMAGE = "ubi"
ALPINE_IMAGE= "alpine"
ifeq ($(ARCH), "s390x")
	RUNTIME_IMAGE = "alpine"
	ALPINE_IMAGE= "s390x/alpine"
endif

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

.PHONY: package
package:
	docker run --rm --entrypoint=/bin/bash \
		-v $(shell pwd)/scripts:$(INSTALL_PATH)/scripts \
		-v $(shell pwd)/modules:$(INSTALL_PATH)/modules/src \
		-v $(shell pwd)/LICENSE.md:$(INSTALL_PATH)/LICENSE.md \
		-v $(shell pwd)/README.md:$(INSTALL_PATH)/README.md \
		sysflowtelemetry/sf-collector-musl:${SYSFLOW_VERSION} -- $(INSTALL_PATH)/scripts/cpack/prepackage.sh
	cd scripts/cpack && export SYSFLOW_VERSION=$(SYSFLOW_VERSION) && cpack --config ./CPackConfig.cmake

.PHONY: package-libs
package-libs:
	docker run --rm --entrypoint=/bin/bash \
		-v $(shell pwd)/scripts:$(INSTALL_PATH)/scripts \
		-v $(shell pwd)/LICENSE.md:$(INSTALL_PATH)/LICENSE.md \
		-v $(shell pwd)/README.md:$(INSTALL_PATH)/README.md \
		sysflowtelemetry/sf-collector-libs:${SYSFLOW_VERSION} -- $(INSTALL_PATH)/scripts/cpack/prepackage-libs.sh
	docker run --rm --entrypoint=/bin/bash \
		-v $(shell pwd)/scripts:$(INSTALL_PATH)/scripts \
		-v $(shell pwd)/modules:$(INSTALL_PATH)/modules/src \
		sysflowtelemetry/sf-collector:${SYSFLOW_VERSION} -- $(INSTALL_PATH)/scripts/cpack/prepackage-driver.sh
	cd scripts/cpack && export SYSFLOW_VERSION=$(SYSFLOW_VERSION) && cpack --config ./CPackConfig-libs.cmake

.PHONY: package-libs/musl
package-libs/musl:
	docker run --rm --entrypoint=/bin/bash \
		-v $(shell pwd)/scripts:$(INSTALL_PATH)/scripts \
		-v $(shell pwd)/LICENSE.md:$(INSTALL_PATH)/LICENSE.md \
		-v $(shell pwd)/README.md:$(INSTALL_PATH)/README.md \
		sysflowtelemetry/sf-collector-libs-musl:${SYSFLOW_VERSION} -- $(INSTALL_PATH)/scripts/cpack/prepackage-libs-musl.sh
	echo "Executing prepackage driver"
	docker run --rm --entrypoint=/bin/bash \
		-v $(shell pwd)/scripts:$(INSTALL_PATH)/scripts \
		-v $(shell pwd)/modules:$(INSTALL_PATH)/modules/src \
		sysflowtelemetry/sf-collector-musl:${SYSFLOW_VERSION} -- $(INSTALL_PATH)/scripts/cpack/prepackage-driver.sh
	echo "Executing cpack"
	cd scripts/cpack && export SYSFLOW_VERSION=$(SYSFLOW_VERSION) && cpack --config ./CPackConfig-libs-musl.cmake

.PHONY: clean
clean:
	make -C src/collector clean
	make -C src/libs clean
	make -C modules clean
	cd scripts/cpack && ./clean.sh

.PHONY: init
init: 
	make -C modules init

.PHONY: build
build: init docker-base-build docker-mods-build docker-driver-build docker-libs-build docker-collector-build docker-runtime-build

.PHONY: build/musl
build/musl: init docker-base-build/musl docker-mods-build/musl docker-driver-build docker-libs-build/musl docker-collector-build/musl docker-runtime-build/musl

.PHONY: docker-base-build
docker-base-build:
	( DOCKER_BUILDKIT=1 docker build --secret id=rhuser,src=$(shell pwd)/scripts/build/rhuser --secret id=rhpassword,src=$(shell pwd)/scripts/build/rhpassword --build-arg UBI_VER=${UBI_VERSION} --target base -t sysflowtelemetry/ubi:base-${FALCO_LIBS_VERSION}-${FALCO_VERSION}-${UBI_VERSION} -f Dockerfile.ubi.amd64 . )

.PHONY: docker-base-build/musl
docker-base-build/musl:
	( DOCKER_BUILDKIT=1 docker build --build-arg ALPINE_IMAGE=${ALPINE_IMAGE} --build-arg ALPINE_VER=${ALPINE_VERSION} --target base -t sysflowtelemetry/alpine:base-${FALCO_LIBS_VERSION}-${FALCO_VERSION}-${ALPINE_VERSION} -f Dockerfile.alpine . )

.PHONY: docker-mods-build
docker-mods-build:
	( DOCKER_BUILDKIT=1 docker build --secret id=rhuser,src=$(shell pwd)/scripts/build/rhuser --secret id=rhpassword,src=$(shell pwd)/scripts/build/rhpassword --build-arg MAKE_JOBS=${MAKE_JOBS} --build-arg UBI_VER=${UBI_VERSION} --target mods -t sysflowtelemetry/ubi:mods-${FALCO_LIBS_VERSION}-${FALCO_VERSION}-${UBI_VERSION} -f Dockerfile.ubi.amd64 . )

.PHONY: docker-mods-build/musl
docker-mods-build/musl:
	( DOCKER_BUILDKIT=1 docker build --build-arg MAKE_JOBS=${MAKE_JOBS} --build-arg ALPINE_IMAGE=${ALPINE_IMAGE} --build-arg ALPINE_VER=${ALPINE_VERSION} --target mods -t sysflowtelemetry/alpine:mods-${FALCO_LIBS_VERSION}-${FALCO_VERSION}-${ALPINE_VERSION} -f Dockerfile.alpine . )

.PHONY: docker-driver-build
docker-driver-build:
	( DOCKER_BUILDKIT=1 docker build --secret id=rhuser,src=$(shell pwd)/scripts/build/rhuser --secret id=rhpassword,src=$(shell pwd)/scripts/build/rhpassword --build-arg MAKE_JOBS=${MAKE_JOBS} --build-arg UBI_VER=${UBI_VERSION} --build-arg FALCO_VER=${FALCO_VERSION} --build-arg FALCO_LIBS_VER=${FALCO_LIBS_VERSION} --target driver -t sysflowtelemetry/ubi:driver-${FALCO_LIBS_VERSION}-${FALCO_VERSION}-${UBI_VERSION} -f Dockerfile.driver.amd64 . )

.PHONY: docker-libs-build
docker-libs-build:
	( DOCKER_BUILDKIT=1 docker build --build-arg UBI_VER=${UBI_VERSION} --build-arg FALCO_VER=${FALCO_VERSION} --build-arg FALCO_LIBS_VER=${FALCO_LIBS_VERSION} --build-arg FALCO_LIBS_DRIVER_VER=${FALCO_LIBS_DRIVER_VERSION} --target libs -t sysflowtelemetry/sf-collector-libs:${SYSFLOW_VERSION} -f Dockerfile . )

.PHONY: docker-libs-build/musl
docker-libs-build/musl:
	( DOCKER_BUILDKIT=1 docker build --build-arg RUNTIME_IMAGE=${RUNTIME_IMAGE} --build-arg ALPINE_VER=${ALPINE_VERSION} --build-arg FALCO_VER=${FALCO_VERSION} --build-arg FALCO_LIBS_VER=${FALCO_LIBS_VERSION} --build-arg FALCO_LIBS_DRIVER_VER=${FALCO_LIBS_DRIVER_VERSION} --target libs -t sysflowtelemetry/sf-collector-libs-musl:${SYSFLOW_VERSION} -f Dockerfile.musl . )

.PHONY: docker-collector-build
docker-collector-build:
	( DOCKER_BUILDKIT=1 docker build --build-arg UBI_VER=${UBI_VERSION} --build-arg FALCO_VER=${FALCO_VERSION} --build-arg FALCO_LIBS_VER=${FALCO_LIBS_VERSION} --build-arg FALCO_LIBS_DRIVER_VER=${FALCO_LIBS_DRIVER_VERSION} --target collector -t sysflowtelemetry/sf-collector-builder:${SYSFLOW_VERSION} -f Dockerfile . )

.PHONY: docker-collector-build/musl
docker-collector-build/musl:
	( DOCKER_BUILDKIT=1 docker build --build-arg ALPINE_VER=${ALPINE_VERSION} --build-arg RUNTIME_IMAGE=${RUNTIME_IMAGE} --build-arg FALCO_VER=${FALCO_VERSION} --build-arg FALCO_LIBS_VER=${FALCO_LIBS_VERSION} --build-arg FALCO_LIBS_DRIVER_VER=${FALCO_LIBS_DRIVER_VERSION} --target collector -t sysflowtelemetry/sf-collector-builder-musl:${SYSFLOW_VERSION} -f Dockerfile.musl . )

.PHONY: docker-runtime-build
docker-runtime-build:
	( DOCKER_BUILDKIT=1 docker build --build-arg UBI_VER=${UBI_VERSION} --build-arg FALCO_VER=${FALCO_VERSION} --build-arg FALCO_LIBS_VER=${FALCO_LIBS_VERSION} --build-arg FALCO_LIBS_DRIVER_VER=${FALCO_LIBS_DRIVER_VERSION} --target runtime -t sysflowtelemetry/sf-collector:${SYSFLOW_VERSION} -f Dockerfile . )

.PHONY: docker-runtime-build/musl
docker-runtime-build/musl:
	( DOCKER_BUILDKIT=1 docker build --build-arg RUNTIME_IMAGE=${RUNTIME_IMAGE} --build-arg ALPINE_VER=${ALPINE_VERSION} --build-arg UBI_VER=${UBI_VERSION} --build-arg FALCO_VER=${FALCO_VERSION} --build-arg FALCO_LIBS_VER=${FALCO_LIBS_VERSION} --build-arg FALCO_LIBS_DRIVER_VER=${FALCO_LIBS_DRIVER_VERSION} --target runtime -t sysflowtelemetry/sf-collector-musl:${SYSFLOW_VERSION} -f Dockerfile.musl . )

.PHONY: docker-driver-build/musl/s390x
docker-driver-build/musl/s390x:
	( DOCKER_BUILDKIT=1 docker build --no-cache --build-arg ALPINE_VER=${ALPINE_VERSION} --build-arg UBI_VER=${UBI_VERSION} --build-arg FALCO_VER=${FALCO_VERSION} --build-arg FALCO_LIBS_VER=${FALCO_LIBS_VERSION} --build-arg FALCO_LIBS_DRIVER_VER=${FALCO_LIBS_DRIVER_VERSION} --target driver -t sysflowtelemetry/alpine:driver-${FALCO_LIBS_VERSION}-${FALCO_VERSION}-${UBI_VERSION} -f Dockerfile.driver.s390x . )

.PHONY: docker-testing-build
docker-testing-build:
	( DOCKER_BUILDKIT=1 docker build --build-arg UBI_VER=${UBI_VERSION} --build-arg FALCO_VER=${FALCO_VERSION} --build-arg FALCO_LIBS_VER=${FALCO_LIBS_VERSION} --build-arg FALCO_LIBS_DRIVER_VER=${FALCO_LIBS_DRIVER_VERSION} --target testing -t sysflowtelemetry/sf-collector-testing:${SYSFLOW_VERSION} -f Dockerfile . )

.PHONY: docker-testing-build/musl
docker-testing-build/musl:
	( DOCKER_BUILDKIT=1 docker build --build-arg RUNTIME_IMAGE=${RUNTIME_IMAGE} --build-arg ALPINE_VER=${ALPINE_VERSION} --build-arg UBI_VER=${UBI_VERSION} --build-arg FALCO_VER=${FALCO_VERSION} --build-arg FALCO_LIBS_VER=${FALCO_LIBS_VERSION} --build-arg FALCO_LIBS_DRIVER_VER=${FALCO_LIBS_DRIVER_VERSION} --target testing -t sysflowtelemetry/sf-collector-testing-musl:${SYSFLOW_VERSION} -f Dockerfile.musl . )

.PHONY: docker-test
docker-test:
	docker run --rm --name sf-test -v $(shell pwd)/tests:/usr/local/sysflow/tests -e INTERVAL=300 -e EXPORTER_ID=tests -e OUTPUT=/mnt/data/ sysflowtelemetry/sf-collector-testing:${SYSFLOW_VERSION} tests/tests.bats

.PHONY: docker-test/musl
docker-test/musl:
	docker run --rm --name sf-test -v $(shell pwd)/tests:/usr/local/sysflow/tests -e INTERVAL=300 -e EXPORTER_ID=tests -e OUTPUT=/mnt/data/ sysflowtelemetry/sf-collector-testing-musl:${SYSFLOW_VERSION} tests/tests.bats

.PHONY: docker-baseline-tests
docker-baseline-tests:
	docker run --rm --name sf-test -v $(shell pwd)/logs:/tmp -v $(shell pwd)/tests:/usr/local/sysflow/tests -e INTERVAL=300 -e EXPORTER_ID=tests -e OUTPUT=/mnt/data/ sysflowtelemetry/sf-collector-testing:${SYSFLOW_VERSION} tests/baseline.bats

.PHONY: docker-baseline-tests/musl
docker-baseline-tests/musl:
	docker run --rm --name sf-test -v $(shell pwd)/logs:/tmp -v $(shell pwd)/tests:/usr/local/sysflow/tests -e INTERVAL=300 -e EXPORTER_ID=tests -e OUTPUT=/mnt/data/ sysflowtelemetry/sf-collector-testing-musl:${SYSFLOW_VERSION} tests/baseline.bats

.PHONY : help
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... modules"
	@echo "... sysporter"
	@echo "... package"
	@echo "... install"
	@echo "... uninstall"
	@echo "... init"
	@echo "... build"
	@echo "... build/musl"
	@echo "... docker-base-build"
	@echo "... docker-base-build/musl"
	@echo "... docker-mods-build"
	@echo "... docker-mods-build/musl"
	@echo "... docker-driver-build"
	@echo "... docker-libs-build"
	@echo "... docker-libs-build/musl"
	@echo "... docker-collector-build"
	@echo "... docker-collector-build/musl"
	@echo "... docker-runtime-build"
	@echo "... docker-runtime-build/musl"
	@echo "... docker-testing-build"
	@echo "... docker-testing-build/musl"
	@echo "... docker-test"
	@echo "... docker-test/musl"
	@echo "... docker-baseline-tests"
	@echo "... docker-baseline-tests/musl"

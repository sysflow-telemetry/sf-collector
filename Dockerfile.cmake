# syntax = docker/dockerfile:1.0-experimental
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

ARG UBI_VER
FROM registry.access.redhat.com/ubi8/ubi:${UBI_VER} AS base

# Install Packages
COPY ./scripts/installUBIDependency.sh /build/install.sh

RUN --mount=type=secret,id=rhuser --mount=type=secret,id=rhpassword \
     ( export REGISTER_USER=$(cat /run/secrets/rhuser) && export REGISTER_PASSWORD=$(cat /run/secrets/rhpassword) && bash /build/install.sh base ) && \
     rm -rf /build

ARG BUILD_TYPE=release
ARG BUILD_DRIVER=OFF
ARG BUILD_BPF=OFF
ARG BUILD_WARNINGS_AS_ERRORS=OFF
ARG MAKE_JOBS=2
ARG FALCO_VERSION=0.32.1
ARG CMAKE_VERSION=3.22.5

ENV BUILD_TYPE=${BUILD_TYPE}
ENV BUILD_DRIVER=${BUILD_DRIVER}
ENV BUILD_BPF=${BUILD_BPF}
ENV BUILD_WARNINGS_AS_ERRORS=${BUILD_WARNINGS_AS_ERRORS}
ENV MAKE_JOBS=${MAKE_JOBS}
ENV FALCO_VERSION=${FALCO_VERSION}
ENV CMAKE_VERSION=${CMAKE_VERSION}

COPY modules/sysflow /build/modules/sysflow
COPY src  /build/src
COPY CMakeLists.txt /build/CMakeLists.txt
COPY cmake /build/cmake

RUN cd /build/ && mkdir build && cd build && \
    cmake \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DCMAKE_INSTALL_PREFIX=/usr/local/sysflow \
    -DBUILD_DRIVER="$BUILD_DRIVER" \
    -DBUILD_BPF="$BUILD_BPF" \
    -DBUILD_WARNINGS_AS_ERRORS="$BUILD_WARNINGS_AS_ERRORS" \
    -DDRAIOS_DEBUG_FLAGS="$DRAIOS_DEBUG_FLAGS" \
    -DBUILD_LIBSINSP_EXAMPLES=OFF \
    -DUSE_BUNDLED_DEPS=ON .. && make -j ${MAKE_JOBS} VERBOSE=1 install

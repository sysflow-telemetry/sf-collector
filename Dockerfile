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

ARG FALCO_VER
ARG FALCO_LIBS_VER
ARG FALCO_LIBS_DRIVER_VER
ARG UBI_VER

#-----------------------
# Stage: Libs
#-----------------------
FROM sysflowtelemetry/ubi:mods-${FALCO_LIBS_VER}-${FALCO_VER}-${UBI_VER} AS libs

# install path build args
ARG INSTALL_PATH=/usr/local/sysflow
ARG MODPREFIX=${INSTALL_PATH}/modules

# environment and build args
ARG BUILD_NUMBER=0
ARG DEBUG=0
ARG ASAN=0

# manifest (need to be copied here to refresh sysflow version)
COPY ./makefile.manifest.inc /build/

# build libsysflow
COPY ./modules/sysflow/avro/avsc  /build/modules/sysflow/avro/avsc
COPY ./modules/sysflow/c\+\+/sysflow/sysflow.hh ${MODPREFIX}/include/sysflow/c\+\+/sysflow/sysflow.hh
COPY ./modules/sysflow/c\+\+/sysflow/avsc_sysflow5.hh ${MODPREFIX}/include/sysflow/c\+\+/sysflow/avsc_sysflow5.hh
COPY ./src/libs /build/src/libs
RUN make -C /build/src/libs \
         SYSFLOW_BUILD_NUMBER=$BUILD_NUMBER \
         LIBLOCALPREFIX=${MODPREFIX} \
         FALCOLOCALLIBPREFIX=${MODPREFIX}/lib/falcosecurity \
         FALCOLOCALINCPREFIX=${MODPREFIX}/include/falcosecurity \
         AVRLOCALLIBPREFIX=${MODPREFIX}/lib \
         AVRLOCALINCPREFIX=${MODPREFIX}/include \
         SFLOCALINCPREFIX=${MODPREFIX}/include/sysflow/c++ \
         FSLOCALINCPREFIX=${MODPREFIX}/include/filesystem \
         SCHLOCALPREFIX=${MODPREFIX}/conf \
         DEBUG=${DEBUG} \
         ASAN=${ASAN} \
         install

#-----------------------
# Stage: Collector
#-----------------------
FROM libs as collector

# environment and build args
ARG BUILD_NUMBER=0
ARG DEBUG=0
ARG ASAN=0

# install path build args
ARG INSTALL_PATH=/usr/local/sysflow
ARG MODPREFIX=${INSTALL_PATH}/modules

# build the collector (sysporter)
COPY ./src/collector /build/src/collector
RUN cd /build/src/collector && \
    make SYSFLOW_BUILD_NUMBER=$BUILD_NUMBER \
         LIBLOCALPREFIX=${MODPREFIX} \
         FALCOLOCALLIBPREFIX=${MODPREFIX}/lib/falcosecurity \
         FALCOLOCALINCPREFIX=${MODPREFIX}/include/falcosecurity \
         AVRLOCALLIBPREFIX=${MODPREFIX}/lib \
         AVRLOCALINCPREFIX=${MODPREFIX}/include \
         SFLOCALINCPREFIX=${MODPREFIX}/include/sysflow/c++ \
         FSLOCALINCPREFIX=${MODPREFIX}/include/filesystem \
         SCHLOCALPREFIX=${MODPREFIX}/conf \
         DEBUG=${DEBUG} \
         ASAN=${ASAN} \
         install

#-----------------------
# Stage: Runtime
#-----------------------
FROM sysflowtelemetry/ubi:driver-${FALCO_LIBS_VER}-${FALCO_VER}-${UBI_VER} AS runtime

# environment variables
ARG interval=30
ENV INTERVAL=$interval

ARG filter=
ENV FILTER=$filter

ENV DRIVERPREFIX=/usr/src/falco-

ARG exporterid="local"
ENV EXPORTER_ID=$exporterid

ARG output=
ENV OUTPUT=$output

ARG cripath=
ENV CRI_PATH=$cripath

ARG critimeout=
ENV CRI_TIMEOUT=$critimeout

ARG stats=
ENV STATS=$stats

ARG gllogtostderr=1
ENV GLOG_logtostderr=$gllogtostderr

# Verbose logging (GLOG_minloglevel must be 0): 1 (DEBUG), 2 (TRACE)
ARG glv=
ENV GLOG_v=$glv

# 0 (INFO), 1 (WARNING), 2 (ERROR)
ARG glminlevel=0
ENV GLOG_minloglevel=$glminlevel

ARG INSTALL_PATH=/usr/local/sysflow

ARG MODPREFIX=${INSTALL_PATH}/modules
ENV HOST_ROOT=/host

ARG sockfile=
ENV SOCK_FILE=

ARG VERSION=dev
ARG RELEASE=dev

ARG nodeip=
ENV NODE_IP=$nodeip

ARG FALCO_VER
ENV FALCO_VERSION=${FALCO_VER}

ARG FALCO_LIBS_VER
ENV FALCO_LIBS_VERSION=${FALCO_LIBS_VER}

ARG FALCO_LIBS_DRIVER_VER
ENV FALCO_LIBS_DRIVER_VERSION=${FALCO_LIBS_DRIVER_VER}

ENV DRIVER_NAME="falco"

ARG samplingRate=
ENV SAMPLING_RATE=$samplingRate

ENV DRIVERS_REPO="https://download.falco.org/driver"

# update labels
LABEL "name"="SysFlow Collector"
LABEL "vendor"="SysFlow"
LABEL "version"="${VERSION}"
LABEL "release"="${RELEASE}"
LABEL "falcolibs-version"="${FALCO_LIBS_VER}"
LABEL "falcodriver-version"="${FALCO_LIBS_DRIVER_VER}"
LABEL "summary"="The SysFlow Collector monitors and collects system call and event information from hosts and exports them in the SysFlow format using Apache Avro object serialization"
LABEL "description"="The SysFlow Collector monitors and collects system call and event information from hosts and exports them in the SysFlow format using Apache Avro object serialization"
LABEL "io.k8s.display-name"="SysFlow Collector"
LABEL "io.k8s.description"="The SysFlow Collector monitors and collects system call and event information from hosts and exports them in the SysFlow format using Apache Avro object serialization"

# update license
COPY ./LICENSE.md /licenses/LICENSE.md

# copy resources
COPY --from=collector ${INSTALL_PATH}/bin/sysporter ${INSTALL_PATH}/bin/sysporter
COPY --from=collector ${INSTALL_PATH}/conf/ ${INSTALL_PATH}/conf/

CMD /usr/local/sysflow/bin/sysporter \
     ${INTERVAL:+-G} $INTERVAL \
     ${OUTPUT:+-w} $OUTPUT \
     ${EXPORTER_ID:+-e} "$EXPORTER_ID" \
     ${FILTER:+-f} "$FILTER" \
     ${CRI_PATH:+-p} ${CRI_PATH} \
     ${CRI_TIMEOUT:+-t} ${CRI_TIMEOUT} \
     ${SOCK_FILE:+-u} ${SOCK_FILE} \
     ${SAMPLING_RATE:+-s} ${SAMPLING_RATE} \
     ${DRIVER_TYPE:+-k} "${DRIVER_TYPE}" \
     ${STATS:+-d} \
     ${MODE:+-m} ${MODE}

#-----------------------
# Stage: Testing
#-----------------------
FROM python:3.11-buster AS testing

# environment and build args
ARG BATS_VERSION=1.8.2

ARG wdir=/usr/local/sysflow
ENV WDIR=$wdir

ARG INSTALL_PATH=/usr/local/sysflow

ENV ENABLE_DROP_MODE=0
ENV ENABLE_PROC_FLOW=0
ENV FILE_ONLY=0
ENV FILE_READ_MODE=0

# Install extra packages for tests
RUN mkdir /tmp/bats && cd /tmp/bats && \
    wget https://github.com/bats-core/bats-core/archive/v${BATS_VERSION}.tar.gz && \
    tar -xzf v${BATS_VERSION}.tar.gz && rm -rf v${BATS_VERSION}.tar.gz && \
    cd bats-core-${BATS_VERSION} && ./install.sh /usr/local && rm -rf /tmp/bats

# install APIs
COPY modules/sysflow/py3 ${INSTALL_PATH}/utils

RUN cd /usr/local/sysflow/utils && \
    python3 -m pip install .

# copy the collector binary
COPY --from=collector ${INSTALL_PATH}/bin/sysporter ${INSTALL_PATH}/bin/

WORKDIR $wdir
ENTRYPOINT ["/usr/local/bin/bats"]

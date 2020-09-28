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

ARG SYSDIG_VER=0.26.7
ARG UBI_VER=8.2.299

#-----------------------
# Stage: builder
#-----------------------
FROM sysflowtelemetry/ubi:mods-${SYSDIG_VER}-${UBI_VER}-noncurses AS builder

# environment and build args
ARG BUILD_NUMBER=0

ARG INSTALL_PATH=/usr/local/sysflow

ARG MODPREFIX=${INSTALL_PATH}/modules

ENV LIBRARY_PATH=/lib64

# build sysporter
COPY ./modules/sysflow/avro/avsc  /build/modules/sysflow/avro/avsc
COPY ./modules/sysflow/c++/sysflow/sysflow.hh ${MODPREFIX}/include/sysflow/c++/sysflow/sysflow.hh
COPY ./modules/sysflow/c++/sysflow/avsc_sysflow2.hh ${MODPREFIX}/include/sysflow/c++/sysflow/avsc_sysflow2.hh
COPY ./src/ /build/src/
RUN cd /build/src && \
    make SYSFLOW_BUILD_NUMBER=$BUILD_NUMBER \
         LIBLOCALPREFIX=${MODPREFIX} \
         SDLOCALLIBPREFIX=${MODPREFIX}/lib \
         SDLOCALINCPREFIX=${MODPREFIX}/include/sysdig \
         AVRLOCALLIBPREFIX=${MODPREFIX}/lib \
         AVRLOCALINCPREFIX=${MODPREFIX}/include \
         SFLOCALINCPREFIX=${MODPREFIX}/include/sysflow/c++ \
         FSLOCALINCPREFIX=${MODPREFIX}/include/filesystem \
         SCHLOCALPREFIX=${MODPREFIX}/conf \
         install && \
    make clean && \
    rm -rf /build

#-----------------------
# Stage: Runtime
#-----------------------
FROM sysflowtelemetry/ubi:base-${SYSDIG_VER}-${UBI_VER}-noncurses AS runtime

# environment variables
ARG interval=30
ENV INTERVAL=$interval

ARG filter=
ENV FILTER=$filter

ARG exporterid="local"
ENV EXPORTER_ID=$exporterid

ARG output=
ENV OUTPUT=$output

ARG cripath=
ENV CRI_PATH=$cripath

ARG critimeout=
ENV CRI_TIMEOUT=$critimeout

ARG debug=
ENV DEBUG=$debug

ARG gllogtostderr=1
ENV GLOG_logtostderr=$gllogtostderr

ARG glv=
ENV GLOG_v=$glv

ARG sysdig_log=
ENV SYSDIG_LOG=$sysdig_log

ARG INSTALL_PATH=/usr/local/sysflow

ARG MODPREFIX=${INSTALL_PATH}/modules
ENV SYSDIG_HOST_ROOT=/host

ARG sockfile=
ENV SOCK_FILE=

ARG VERSION=dev
ARG RELEASE=dev

ARG nodeip=
ENV NODE_IP=$nodeip

ARG BPF_PROBE
ENV SYSDIG_BPF_PROBE=${BPF_PROBE:+""}

ARG SYSDIG_VER
ENV SYSDIG_VERSION=${SYSDIG_VER}


# Update Label
LABEL "name"="SysFlow Collector"
LABEL "vendor"="IBM"
LABEL "version"="${VERSION}"
LABEL "release"="${RELEASE}"
LABEL "summary"="The SysFlow Collector monitors and collects system call and event information from hosts and exports them in the SysFlow format using Apache Avro object serialization"
LABEL "description"="The SysFlow Collector monitors and collects system call and event information from hosts and exports them in the SysFlow format using Apache Avro object serialization"
LABEL "io.k8s.display-name"="SysFlow Collector"
LABEL "io.k8s.description"="The SysFlow Collector monitors and collects system call and event information from hosts and exports them in the SysFlow format using Apache Avro object serialization"

# Update License
COPY ./LICENSE.md /licenses/LICENSE.md

# copy dependencies
COPY --from=builder /usr/local/lib/ /usr/local/lib/
COPY --from=builder /sysdigsrc/ /usr/src/
COPY --from=builder ${MODPREFIX}/lib/*.so* ${MODPREFIX}/lib/
COPY --from=builder ${MODPREFIX}/bin/ ${MODPREFIX}/bin/
RUN ln -s ${MODPREFIX}/bin/sysdig-probe-loader /usr/bin/sysdig-probe-loader
RUN ln -s ${MODPREFIX}/bin/sysdig /usr/bin/sysdig
COPY --from=builder ${INSTALL_PATH}/conf/ ${INSTALL_PATH}/conf/
COPY --from=builder ${INSTALL_PATH}/bin/sysporter ${INSTALL_PATH}/bin/
COPY ./docker-entry-ubi.sh /usr/local/sysflow/modules/bin/

# RUN dnf install -y procps net-tools
# entrypoint
WORKDIR /usr/local/sysflow/bin/

ENTRYPOINT ["/usr/local/sysflow/modules/bin/docker-entry-ubi.sh"]

CMD /usr/local/sysflow/bin/sysporter \
    ${INTERVAL:+-G} $INTERVAL \
    ${OUTPUT:+-w} $OUTPUT \
    ${EXPORTER_ID:+-e} "$EXPORTER_ID" \
    ${FILTER:+-f} "$FILTER" \
    ${CRI_PATH:+-p} ${CRI_PATH} \
    ${CRI_TIMEOUT:+-t} ${CRI_TIMEOUT} \
    ${SOCK_FILE:+-u} ${SOCK_FILE} \
    ${DEBUG:+-d}

#-----------------------
# Stage: Testing
#-----------------------
FROM runtime AS testing

# environment and build args
ARG BATS_VERSION=1.1.0

ARG wdir=/usr/local/sysflow
ENV WDIR=$wdir

ARG INSTALL_PATH=/usr/local/sysflow

# Install extra packages for tests
COPY ./scripts/installUBIDependency.sh /
RUN /installUBIDependency.sh test-extra && rm /installUBIDependency.sh

RUN mkdir /tmp/bats && cd /tmp/bats && \
    wget https://github.com/bats-core/bats-core/archive/v${BATS_VERSION}.tar.gz && \
    tar -xzf v${BATS_VERSION}.tar.gz && rm -rf v${BATS_VERSION}.tar.gz && \
    cd bats-core-${BATS_VERSION} && ./install.sh /usr/local && rm -rf /tmp/bats

# install APIs
COPY modules/sysflow/py3 ${INSTALL_PATH}/utils

RUN cd /usr/local/sysflow/utils && \
    python3 setup.py install

WORKDIR $wdir
ENTRYPOINT ["/usr/local/bin/bats"]

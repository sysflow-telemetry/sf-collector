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


#-----------------------
# Stage: deps
#-----------------------
FROM ubuntu:18.04 as deps

# dependencies
RUN apt-get update -yqq && \
    apt-get --fix-broken install -yqq && \
    apt-get upgrade -yqq && \
    apt-get install -yqq \
        patch \
        base-files \
        binutils \
        bzip2 \
        libdpkg-perl \
        perl \
        make \
        xz-utils \
        libncurses5-dev \
        libncursesw5-dev \
        cmake \
        libboost-all-dev \
        g++ \
        flex \ 
        bison \
        wget \
        libelf-dev \
        liblog4cxx-dev \
        libapr1 \
        pkg-config \
        libaprutil1 \
        libsparsehash-dev && \ 
    apt-get clean -yqq && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/* /var/lib/apt/archive/*

COPY  ./modules /build/modules
COPY  ./makefile.* /build/
RUN cd /build/modules && make install

#-----------------------
# Stage: Builder
#-----------------------
FROM deps as builder
ARG BUILD_NUMBER=0

# copy dependencies
COPY --from=deps /build /build/
COPY --from=deps /usr/local/lib/ /usr/local/lib/
COPY --from=deps /usr/local/sysflow/modules/ /usr/local/sysflow/modules/

# build sysporter
COPY ./src/ /build/src/
RUN cd /build/src && make SYSFLOW_BUILD_NUMBER=$BUILD_NUMBER

#-----------------------
# Stage: Runtime
#-----------------------
FROM sysdig/sysdig:0.26.4 as runtime

RUN apt-get update -yqq && \
    apt-get --no-install-recommends --fix-broken install -yqq && \
    apt-get install -yqq  valgrind && \
    apt-get clean -yqq && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/* /var/lib/apt/archive/*

# environment variables
ARG interval=30
ENV INTERVAL=$interval

ARG filter=""
ENV FILTER=$filter

ARG stats=
ENV STATS=$stats

ARG exporterid="local"
ENV EXPORTER_ID=$exporterid

ARG output=/mnt/data/
ENV OUTPUT=$output

COPY --from=builder /usr/local/lib/ /usr/local/lib/
COPY --from=builder /usr/local/sysflow/modules/ /usr/local/sysflow/modules/
COPY --from=builder /usr/lib/x86_64-linux-gnu/libboost*.so* /usr/lib/x86_64-linux-gnu/
COPY --from=builder /usr/lib/x86_64-linux-gnu/liblog4cxx*.so* /usr/lib/x86_64-linux-gnu/
COPY --from=builder /usr/lib/x86_64-linux-gnu/libapr* /usr/lib/x86_64-linux-gnu/
COPY --from=builder /usr/lib/x86_64-linux-gnu/libexpat* /usr/lib/x86_64-linux-gnu/
COPY --from=builder /lib/x86_64-linux-gnu/libexpat* /lib/x86_64-linux-gnu/
COPY --from=builder /build/src/sysporter /usr/local/sysflow/bin/
COPY --from=builder /build/modules/sysflow/avro/avsc/SysFlow.avsc /usr/local/sysflow/conf/
COPY --from=builder /build/src/conf/log4cxx.properties /usr/local/sysflow/conf/
COPY ./runTimeout /
RUN ln -s /usr/local/sysflow/modules/lib/libcrypto.so /usr/local/sysflow/modules/lib/libcrypto.so.1.0.0
RUN ln -s /usr/local/sysflow/modules/lib/libssl.so /usr/local/sysflow/modules/lib/libssl.so.1.0.0

# entrypoint
WORKDIR /usr/local/sysflow/bin/
#CMD /usr/local/sysflow/bin/sysporter -G $INTERVAL -w $OUTPUT -e $NODE_NAME $FILTER $STATS
#CMD /runTimeout $INTERVAL $OUTPUT $NODE_NAME "$FILTER" $STATS
CMD /usr/local/sysflow/bin/sysporter -G $INTERVAL -w $OUTPUT -e "$EXPORTER_ID" -f "$FILTER"

#-----------------------
# Stage: Testing
#-----------------------
FROM runtime as testing

ARG wdir=/usr/local/sysflow
ENV WDIR=$wdir

# dependencies
RUN apt-get update -yqq && \
    apt-get --no-install-recommends --fix-broken install -yqq && \
    apt-get install -yqq \
        apt-utils \
        git \
        locales \
        valgrind \
        python3 \
        python3-pip && \
    locale-gen en_US.UTF-8 && \
    apt-get clean -yqq && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/* /var/lib/apt/archive/*

COPY --from=builder /build/modules/sysflow/py3 /usr/local/sysflow/utils/

RUN cd /usr/local/sysflow/utils && \
    python3 setup.py install 

RUN mkdir /bats && git clone https://github.com/bats-core/bats-core.git /bats && \
    cd /bats && ./install.sh /usr/local && rm -r /bats

WORKDIR $wdir
ENTRYPOINT ["/usr/local/bin/bats"]

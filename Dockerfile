FROM ubuntu:16.04 as deps

# dependencies
RUN apt-get update -yqq && \
    apt-get --fix-broken install -yqq && \
    apt-get upgrade -yqq && \
    apt-get install -yqq \
        apt-utils \
        build-essential \
        libncurses5-dev \
        libncursesw5-dev \
        cmake \
        libboost-all-dev \
        flex \ 
        bison \
        g++ \
        wget \
        libelf-dev \
        linux-headers-$(uname -r) && \
    apt-get clean -yqq && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/* /var/lib/apt/archive/*

COPY  ./modules /build/modules
COPY  ./makefile.* /build/
RUN cd /build/modules && make -j3 install && make clean 

#-----------------------
# Stage: Builder
#-----------------------
FROM ubuntu:16.04 as builder

# dependencies
RUN apt-get update -yqq && \
    apt-get --fix-broken install -yqq && \
    apt-get upgrade -yqq && \
    apt-get install software-properties-common -yqq && \
    add-apt-repository ppa:jonathonf/gcc -y && \
    apt-get update -yqq && \
    apt-get install -yqq \
        apt-utils \
        make \
        libboost-all-dev \
        g++-8 \
        libelf-dev \
        liblog4cxx-dev \
        libsparsehash-dev && \ 
    ln -s /usr/bin/g++-8 /usr/bin/g++ && \
    apt-get clean -yqq && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/* /var/lib/apt/archive/*

# copy dependencies
#COPY --from=deps /usr/local/include/avro/ /usr/local/include/avro/
#COPY --from=deps /usr/local/include/sysdig/ /usr/local/include/sysdig/
#COPY --from=deps /usr/local/lib/ /usr/local/lib/
COPY --from=deps /usr/local/lib/ /usr/local/lib/
COPY --from=deps /usr/local/sysflow/modules/ /usr/local/sysflow/modules/

# build sysporter
COPY ./src/ /build/
RUN cd /build && make

#-----------------------
# Stage: Runtime
#-----------------------
FROM sysdig/sysdig:0.24.2 as runtime

# environment variables
ARG interval=30
ENV INTERVAL=$interval

ARG filter=
ENV FILTER=$filter

ARG prefix=
ENV PREFIX=$prefix

ARG nodename=
ENV NODE_NAME=$nodename

ARG wdir=/mnt/data
ENV WDIR=$wdir

# runtime dependencies
#RUN apt-get update -yq && \
#    apt-get --no-install-recommends --fix-broken install -yq && \
#    apt-get --no-install-recommends install -yqq \
#        libsparsehash-dev && \
#    apt-get clean && \
#    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/* /var/lib/apt/archive/*

#COPY --from=builder /usr/local/include/avro/ /usr/local/include/avro/
#COPY --from=builder /usr/local/include/sysdig/ /usr/local/include/sysdig/
COPY --from=builder /usr/local/lib/ /usr/local/lib/
COPY --from=builder /usr/local/sysflow/modules/ /usr/local/sysflow/modules/
COPY --from=builder /usr/lib/x86_64-linux-gnu/libboost*.so* /usr/lib/x86_64-linux-gnu/
COPY --from=builder /usr/lib/x86_64-linux-gnu/liblog4cxx*.so* /usr/lib/x86_64-linux-gnu/
COPY --from=builder /lib/x86_64-linux-gnu/libssl.so.1.0.0/ /lib/x86_64-linux-gnu/
COPY --from=builder /lib/x86_64-linux-gnu/libcrypto.so.1.0.0 /lib/x86_64-linux-gnu/
COPY --from=builder /build/sysporter /usr/local/sysflow/bin/
COPY --from=builder /build/avro/avsc/SysFlow.avsc /usr/local/sysflow/conf/

# entrypoint
WORKDIR /usr/local/sysflow/bin/
CMD /usr/local/sysflow/bin/sysporter -G $INTERVAL -w $WDIR -e $NODE_NAME $FILTER $PREFIX

#-----------------------
# Stage: Testing
#-----------------------
FROM sysdig/sysdig:0.24.2 as testing

ARG wdir=/usr/local/sysflow
ENV WDIR=$wdir

# dependencies
RUN apt-get update -yqq && \
    apt-get --no-install-recommends --fix-broken install -yqq && \
    apt-get install -yqq \
        apt-utils \
        git \
        locales \
        python3 \
        python3-pip && \
    locale-gen en_US.UTF-8 && \
    apt-get clean -yqq && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/* /var/lib/apt/archive/*

RUN mkdir /bats && git clone https://github.com/bats-core/bats-core.git /bats && \
    cd /bats && ./install.sh /usr/local && rm -r /bats

#COPY --from=builder /usr/local/include/avro/ /usr/local/include/avro/
#COPY --from=builder /usr/local/include/sysdig/ /usr/local/include/sysdig/
COPY --from=builder /usr/local/lib/ /usr/local/lib/
COPY --from=builder /usr/local/sysflow/modules/ /usr/local/sysflow/modules/
COPY --from=builder /usr/lib/x86_64-linux-gnu/libboost*.so* /usr/lib/x86_64-linux-gnu/
COPY --from=builder /lib/x86_64-linux-gnu/libssl.so.1.0.0/ /lib/x86_64-linux-gnu/
COPY --from=builder /lib/x86_64-linux-gnu/libcrypto.so.1.0.0 /lib/x86_64-linux-gnu/
COPY --from=builder /build/sysporter /usr/local/sysflow/bin/
COPY --from=builder /build/avro/avsc/SysFlow.avsc /usr/local/sysflow/conf/
COPY --from=builder /build/avro/py3 /usr/local/sysflow/utils/

RUN cd /usr/local/sysflow/utils && \
    python3 setup.py install 

WORKDIR $wdir
ENTRYPOINT ["/usr/local/bin/bats"]

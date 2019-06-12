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
        libelf-dev && \
    apt-get clean -yqq && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/* /var/lib/apt/archive/*
#        linux-headers-$(uname -r) && \
COPY  ./modules /build/modules
COPY  ./makefile.* /build/
RUN cd /build/modules && make install

#-----------------------
# Stage: Builder
#-----------------------
FROM ubuntu:16.04 as builder
ARG TRAVIS_BUILD_NUMBER

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
        libapr1 \
        libaprutil1 \
        libsparsehash-dev && \ 
    ln -s /usr/bin/g++-8 /usr/bin/g++ && \
    apt-get clean -yqq && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/* /var/lib/apt/archive/*

# copy dependencies
COPY --from=deps /build /build/
COPY --from=deps /usr/local/lib/ /usr/local/lib/
COPY --from=deps /usr/local/sysflow/modules/ /usr/local/sysflow/modules/

# build sysporter
COPY ./src/ /build/src/
RUN cd /build/src && make SYSFLOW_BUILD_NUMBER=$TRAVIS_BUILD_NUMBER

#-----------------------
# Stage: Runtime
#-----------------------
FROM sysdig/sysdig:0.24.2 as runtime

# environment variables
ARG interval=30
ENV INTERVAL=$interval

ARG filter=
ENV FILTER=$filter

ARG nodename=
ENV NODE_NAME=$nodename

ARG wdir=/mnt/data
ENV WDIR=$wdir

COPY --from=builder /usr/local/lib/ /usr/local/lib/
COPY --from=builder /usr/local/sysflow/modules/ /usr/local/sysflow/modules/
COPY --from=builder /usr/lib/x86_64-linux-gnu/libboost*.so* /usr/lib/x86_64-linux-gnu/
COPY --from=builder /usr/lib/x86_64-linux-gnu/liblog4cxx*.so* /usr/lib/x86_64-linux-gnu/
COPY --from=builder /usr/lib/x86_64-linux-gnu/libapr* /usr/lib/x86_64-linux-gnu/
COPY --from=builder /usr/lib/x86_64-linux-gnu/libexpat* /usr/lib/x86_64-linux-gnu/
COPY --from=builder /lib/x86_64-linux-gnu/libssl.so.1.0.0/ /lib/x86_64-linux-gnu/
COPY --from=builder /lib/x86_64-linux-gnu/libcrypto.so.1.0.0 /lib/x86_64-linux-gnu/
COPY --from=builder /lib/x86_64-linux-gnu/libexpat* /lib/x86_64-linux-gnu/
COPY --from=builder /build/src/sysporter /usr/local/sysflow/bin/
COPY --from=builder /build/src/avro/avsc/SysFlow.avsc /usr/local/sysflow/conf/
COPY --from=builder /build/src/conf/log4cxx.properties /usr/local/sysflow/conf/

# entrypoint
WORKDIR /usr/local/sysflow/bin/
CMD /usr/local/sysflow/bin/sysporter -G $INTERVAL -w $WDIR -e $NODE_NAME $FILTER

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

COPY --from=runtime /usr/local/lib/ /usr/local/lib/
COPY --from=runtime /usr/local/sysflow /usr/local/sysflow
COPY --from=builder /build/src/avro/py3 /usr/local/sysflow/utils/
COPY --from=builder /usr/lib/x86_64-linux-gnu/libboost*.so* /usr/lib/x86_64-linux-gnu/
COPY --from=builder /usr/lib/x86_64-linux-gnu/liblog4cxx*.so* /usr/lib/x86_64-linux-gnu/
COPY --from=builder /usr/lib/x86_64-linux-gnu/libapr* /usr/lib/x86_64-linux-gnu/
COPY --from=builder /usr/lib/x86_64-linux-gnu/libexpat* /usr/lib/x86_64-linux-gnu/
COPY --from=builder /lib/x86_64-linux-gnu/libssl.so.1.0.0/ /lib/x86_64-linux-gnu/
COPY --from=builder /lib/x86_64-linux-gnu/libexpat* /lib/x86_64-linux-gnu/
COPY --from=builder /lib/x86_64-linux-gnu/libcrypto.so.1.0.0 /lib/x86_64-linux-gnu/

RUN cd /usr/local/sysflow/utils && \
    python3 setup.py install 

WORKDIR $wdir
ENTRYPOINT ["/usr/local/bin/bats"]

FROM ubuntu:16.04 as deps

# dependencies
RUN apt-get update -yq && \
    apt-get --fix-broken install -yq && \
    apt-get upgrade -yq && \
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
        #libsparsehash-dev \
        libelf-dev \
        linux-headers-$(uname -r) && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/* /var/lib/apt/archive/*

COPY  ./src/modules /build/modules
COPY  ./src/makefile.* /build/
RUN cd /build/modules && make modules && make clean 

#-----------------------
# Stage: Builder
#-----------------------
FROM ubuntu:16.04 as builder

# dependencies
RUN apt-get update -yq && \
    apt-get --fix-broken install -yq && \
    apt-get upgrade -yq && \
    apt-get install software-properties-common -yq && \
    add-apt-repository ppa:jonathonf/gcc -y && \
    apt-get update && \
    apt-get install -yqq \
        apt-utils \
        make \
        libboost-all-dev \
        g++-8 \
        libelf-dev && \
        #libsparsehash-dev && \ 
    ln -s /usr/bin/g++-8 /usr/bin/g++ && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/* /var/lib/apt/archive/*

# copy dependencies
COPY --from=deps /usr/local/include/avro/ /usr/local/include/avro/
COPY --from=deps /usr/local/include/sysdig/ /usr/local/include/sysdig/
COPY --from=deps /usr/local/lib/ /usr/local/lib/

# build sysporter
COPY ./src/ /build/
RUN cd /build && \
    make sysporter && \
    make sysporter_install && \
    make clean

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

COPY --from=builder /usr/local/include/avro/ /usr/local/include/avro/
COPY --from=builder /usr/local/include/sysdig/ /usr/local/include/sysdig/
COPY --from=builder /usr/local/lib/ /usr/local/lib/
COPY --from=builder /usr/lib/x86_64-linux-gnu/libboost*.so* /usr/lib/x86_64-linux-gnu/
COPY --from=builder /lib/x86_64-linux-gnu/libssl.so.1.0.0/ /lib/x86_64-linux-gnu/
COPY --from=builder /lib/x86_64-linux-gnu/libcrypto.so.1.0.0 /lib/x86_64-linux-gnu/
COPY --from=builder /usr/local/sysflow/ /usr/local/sysflow/
#COPY --from=builder /build/sysporter /usr/local/sysflow/bin/
#COPY --from=builder /build/avro/avsc/SysFlow.avsc /usr/local/sysflow/conf/

# entrypoint
WORKDIR /usr/local/sysflow/bin/
CMD /usr/local/sysflow/bin/sysporter -G $INTERVAL -w $WDIR -e $NODE_NAME $FILTER $PREFIX

#-----------------------
# Stage: Testing
#-----------------------
FROM ubuntu:16.04 as testing

ENV LANG='en_US.UTF-8' LANGUAGE='en_US:en' LC_ALL='en_US.UTF-8'

# dependencies
RUN apt-get update -yq && \
    apt-get upgrade -yq && \
    apt-get install -yqq \
        apt-utils \
        build-essential \
        libncurses5-dev \
        libncursesw5-dev \
        cmake \
        libboost-all-dev \
        flex \ 
        bison \
        wget \
        #libsparsehash-dev \
        libelf-dev \
        locales \
        python3 \
        python3-pip && \
    locale-gen en_US.UTF-8 && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/* /var/lib/apt/archive/*

COPY --from=builder /usr/local/sysflow/ /usr/local/sysflow/
#COPY --from=builder /build/sysporter /usr/local/sysflow/bin/
#COPY --from=builder /build/avro/avsc/SysFlow.avsc /usr/local/sysflow/conf/
COPY --from=builder /build/avro/py3 /usr/local/sysflow/utils/
COPY  ./tests/ /usr/local/sysflow/tests/

RUN cd /usr/local/sysflow/utils && \
    python3 setup.py install 

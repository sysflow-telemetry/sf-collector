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
        libsparsehash-dev \
        libelf-dev \
        linux-headers-$(uname -r) && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/* /var/lib/apt/archive/*


COPY  ./src/ /build/
RUN cd /build/modules && make modules 

#-----------------------
# Stage: Builder
#-----------------------
#FROM sysdig/sysdig:0.24.2 as builder
FROM ubuntu:16.04 as builder

# dependencies
RUN apt-get update -yq && \
    apt-get --fix-broken install -yq && \
    apt-get upgrade -yq && \
    apt-get install software-properties-common -yq && \
    add-apt-repository ppa:jonathonf/gcc && \
    apt-get update && \
    apt-get install -yqq \
        apt-utils \
        build-essential \
        libboost-all-dev \
        g++-8 \
        libsparsehash-dev && \ 
    apt-get clean && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/* /var/lib/apt/archive/*

# build sysdig
#ADD https://github.com/draios/sysdig/archive/0.24.2.tar.gz /
#RUN mkdir /sysdig/build && \ 
#    cd /sysdig/build && \ 
#    cmake ../. && \ 
#    make -j3 openssl driver scap sinsp b64 curl jq tbb zlib

# build avro
#ADD ./libs/avro/avro-cpp-1.8.2.tar.gz  /
#RUN cd /avro-cpp-1.8.2/ && \
#    cmake -DCMAKE_CXX_FLAGS_ALL_WARNINGS:STRING="-w" \
#          -DCMAKE_BUILD_TYPE=ALL_WARNINGS -G "Unix Makefiles" . && \
#    make -j3 install

#COPY  ./src/ /sysporter/
#COPY  ./libs/sysdig/ /libs/sysdig/

#COPY /lib/modules lib/modules

# copy dependencies
COPY --from=deps /usr/local/include/avro/ /usr/local/include/avro/
COPY --from=deps /usr/local/include/sysdig/ /usr/local/include/sysdig/
COPY --from=deps /usr/local/lib/ /usr/local/lib/

# build sysporter
COPY ./src/ /build/
RUN cd /build && make sysporter
#RUN cd sysporter/ && make
#RUN mkdir -p /usr/local/sysflow/bin
#RUN mkdir -p /usr/local/sysflow/conf
#RUN cp /sysporter/sysporter /usr/local/sysflow/bin
#RUN cp /sysporter/sysreader /usr/local/sysflow/bin
#RUN cp /sysporter/avro/avsc/SysFlow.avsc /usr/local/sysflow/conf/

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
RUN apt-get update -yq && \
    apt-get --no-install-recommends --fix-broken install -yq && \
    apt-get --no-install-recommends install -yqq \
#        libboost1.67 \
        flex \ 
        bison \
        openssl \
        #libssl1.0.0 \
        libsparsehash-dev && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/* /var/lib/apt/archive/*

COPY --from=builder /usr/local/include/avro/ /usr/local/include/avro/
COPY --from=builder /usr/local/include/sysdig/ /usr/local/include/sysdig/
COPY --from=builder /usr/local/lib/ /usr/local/lib/
#COPY --from=builder /usr/local/include/avro/ /usr/local/include/avro/
#COPY --from=builder /usr/local/lib/libavro* /usr/local/lib/
#COPY --from=builder /usr/local/sysflow/ /usr/local/sysflow/
#COPY --from=builder /libs/sysdig/* /usr/local/lib/

# entrypoint
WORKDIR /usr/local/sysflow/bin/
CMD /usr/local/sysflow/bin/sysporter -G $INTERVAL -w $WDIR -e $NODE_NAME $FILTER $PREFIX

#-----------------------
# Stage: Testing
#-----------------------


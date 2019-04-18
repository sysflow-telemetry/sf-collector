FROM sysdig/sysdig:0.24.2

# environment variables
ARG interval=30
ENV INTERVAL=$interval

ARG filter=
ENV FILTER=$filter

ARG prefix=
ENV PREFIX=$prefix

ARG nodename=
ENV NODE_NAME=$nodename

ARG nodeip=
ENV NODE_IP=$nodeip

ARG podname=
ENV POD_NAME=$podname

ARG podnamespace=
ENV POD_NAMESPACE=$podnamespace

ARG podip=
ENV POD_IP=$podip

ARG podserviceaccount=
ENV POD_SERVICE_ACCOUNT=$podserviceaccount

ARG poduuid=
ENV POD_UUID=$poduuid

ARG dir=/mnt/data
ENV DIR=$dir

ENV LD_LIBRARY_PATH=/usr/local/lib

# dependencies
RUN apt-get update -yq && \
    apt-get --no-install-recommends --fix-broken install -yq && \
    apt-get --no-install-recommends install -yqq \
        cmake \
        libboost-all-dev \
        flex \ 
        bison \
        g++ \
        libsparsehash-dev && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/* /var/lib/apt/archive/*

ADD ./libs/avro/avro-cpp-1.8.2.tar.gz  /
RUN cd /avro-cpp-1.8.2/ && \
    cmake -DCMAKE_CXX_FLAGS_ALL_WARNINGS:STRING="-w" \
          -DCMAKE_BUILD_TYPE=ALL_WARNINGS -G "Unix Makefiles" . && \
    make install

COPY  ./src/ /sysporter/
COPY  ./libs/sysdig/include/ /libs/sysdig/include/
COPY  ./libs/sysdig/lib/ /libs/sysdig/lib/

# build sysporter
RUN cd sysporter/ && make
RUN mkdir -p /usr/local/sysflow/bin
RUN mkdir -p /usr/local/sysflow/conf
RUN cp /sysporter/sysporter /usr/local/sysflow/bin
RUN cp /sysporter/sysreader /usr/local/sysflow/bin
RUN cp /sysporter/avro/avsc/SysFlow.avsc /usr/local/sysflow/conf/

# clean up sources
RUN rm -rf /sysporter && \
    rm /avro-cpp-1.8.2.tar.gz && \
    rm -rf /avro-cpp-1.8.2/ && \
    rm -rf /include-sysdig/ && \
    rm -rf /lib-sysdig/

# entrypoint
WORKDIR /usr/local/sysflow/bin/
CMD /usr/local/sysflow/bin/sysporter -G $INTERVAL -w $DIR -e $NODE_NAME $FILTER $PREFIX

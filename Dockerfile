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
RUN apt-get update -yq && apt-get --fix-broken install -yq && apt-get install -yqq \
    cmake \
    libboost-all-dev \
    flex \ 
    bison \
    g++ \
    libsparsehash-dev

COPY ./avro-cpp-1.8.2.tar.gz  /
RUN tar xvf /avro-cpp-1.8.2.tar.gz
RUN cd /avro-cpp-1.8.2/ && \
    cmake -E env CXXFLAGS="-w" cmake -G "Unix Makefiles" . && \
    make install

#RUN mkdir -p /sysporter/
#RUN mkdir -p /include-sysdig/ 
#RUN mkdir -p /lib-sysdig/ 

COPY  ./src/ /sysporter/
COPY  ./include-sysdig/ /include-sysdig/
COPY  ./lib-sysdig/ /lib-sysdig/

# build sysporter
RUN cd sysporter/ && make
RUN cp /lib-sysdig/* /usr/lib/
#RUN mkdir -p /usr/local/sysflow/bin
#RUN mkdir -p /usr/local/sysflow/conf
RUN cp /sysporter/sysporter /usr/local/sysflow/bin
RUN cp /sysporter/sysreader /usr/local/sysflow/bin
RUN cp /sysporter/avro_union/avsc/SysFlow.avsc /usr/local/sysflow/conf/

# clean up sources
RUN rm -rf /sysporter && \
    rm /avro-cpp-1.8.2.tar.gz && \
    rm -rf /avro-cpp-1.8.2/ && \
    rm -rf ./sysdiginclude/ && \
    rm -rf ./libsysdig/

# clean up APT when done
RUN apt-get purge -yq cmake g++
RUN apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/* /var/lib/apt/archive/*

# entrypoint
WORKDIR /usr/local/sysflow/bin/
CMD /usr/local/sysflow/bin/sysporter -G $INTERVAL -w $DIR -e $NODE_NAME $FILTER $PREFIX

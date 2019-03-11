FROM sysdig/sysdig:0.23.0

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
RUN apt-get update
RUN apt --fix-broken install -y
RUN apt-get install cmake -y
RUN apt-get install libboost-all-dev flex bison g++ -y
COPY ./avro-cpp-1.8.2.tar.gz  /
RUN tar xvf /avro-cpp-1.8.2.tar.gz
RUN cd /avro-cpp-1.8.2/ && cmake -G "Unix Makefiles" && make install
#ARG filter="container.type!=host and container.type=docker and container.name!=registry and container.name!=monitor and not(container.name contains trace_collector)"
#ENV FILTER=$filter
RUN apt-get install libsparsehash-dev -y
RUN mkdir -p /sysporter/
RUN mkdir -p /include-sysdig/ 
RUN mkdir -p /lib-sysdig/ 
#COPY sysflow.tar.gz /sysporter/
COPY  ./src/ /sysporter/
COPY  ./include-sysdig/ /include-sysdig/
COPY  ./lib-sysdig/ /lib-sysdig/
#RUN  ls -la /sysporter/*
#RUN  ls -la /sysdiginclude/*
#RUN  ls -la /libsysdig/*
RUN cd sysporter/ && make
RUN cp /lib-sysdig/* /usr/lib/
RUN mkdir -p /usr/local/sysflow/bin
RUN mkdir -p /usr/local/sysflow/conf
RUN cp /sysporter/sysporter /usr/local/sysflow/bin
RUN cp /sysporter/sysreader /usr/local/sysflow/bin
RUN cp /sysporter/avro_union/avsc/SysFlow.avsc /usr/local/sysflow/conf/
RUN rm -rf /sysporter && rm /avro-cpp-1.8.2.tar.gz && rm -rf /avro-cpp-1.8.2/ && rm -rf ./sysdiginclude/ && rm -rf ./libsysdig/
RUN apt-get purge -y cmake g++
#COPY ./sysporter /
#COPY ./openssl/lib/lib* /usr/lib/
#COPY ./libavro/lib* /usr/lib/
# entrypoint
WORKDIR /usr/local/sysflow/bin/
CMD /usr/local/sysflow/bin/sysporter -G $INTERVAL -w $DIR -e $NODE_NAME $FILTER $PREFIX
#CMD sleep 3600

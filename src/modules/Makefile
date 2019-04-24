SYSDIG_VERSION=0.24.2
AVRO_VERSION=release-1.8.2

.PHONY: all
all: init \
	 sysdig \
	 avro

.PHONY: init
init:
	git submodule update --init --recursive

.PHONY: sysdig
sysdig: 
	cd sysdig && git checkout $(SYSDIG_VERSION) && \
	mkdir -p build && cd build && \
	cmake ../. && make -j3 openssl driver scap sinsp b64 curl jq tbb zlib 

.PHONY: avro
avro:
	cd avro && git checkout $(AVRO_VERSION) && \
	cd lang/c++ && mkdir -p build && cd build && \
	cmake -DCMAKE_CXX_FLAGS_ALL_WARNINGS:STRING="-w" \
	      -DCMAKE_BUILD_TYPE=ALL_WARNINGS -G "Unix Makefiles" ../. && \
    make -j3 install

.PHONY: clean
clean:
	rm -rf avro/lang/c++/build sysdig/build

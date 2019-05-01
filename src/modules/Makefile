include ../makefile.manifest.inc
include ../makefile.env.inc

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
	sed -i -E "s/SYSDIG_VERSION \".+\"/SYSDIG_VERSION \"$(SYSDIG_VERSION)\"/" CMakeLists.txt \
	mkdir -p build && cd build && \
    cmake ../. && make -j3 openssl driver scap sinsp b64 curl jq tbb zlib && \
	mkdir -p $(SDINCLUDE) && mkdir -p $(SDLIB) && \
	cp curl-prefix/src/curl/include/curl/*.h $(SDINCLUDE)/curl/ && \
	cp driver/*h $(SDINCLUDE)/driver && \
	cp ../driver/*h $(SDINCLUDE)/driver && \
	cp ../userspace/libsinsp/third-party/jsoncpp/jsoncpp.cpp $(SDINCLUDE)/json && \
	cp ../userspace/libsinsp/third-party/jsoncpp/json/*.h $(SDINCLUDE)/json && \
	cp ../userspace/common/*.h $(SDINCLUDE)/userspace/common && \
	cp ../userspace/libsinsp/*.h $(SDINCLUDE)/userspace/libsinsp && \
	cp ../userspace/libscap/*.h $(SDINCLUDE)/userspace/libscap && \
	cp userspace/libsinsp/libsinsp.a $(SDLIB)/ && \
	cp userspace/libscap/libscap.a $(SDLIB)/ && \
	cp b64-prefix/src/b64/src/libb64.a $(SDLIB)/ && \
	cp curl-prefix/src/curl/lib/.libs/libcurl.a $(SDLIB)/ && \
	cp jq-prefix/src/jq/.libs/libjq.a $(SDLIB)/ && \
	cp tbb-prefix/src/tbb/build/lib_release/libtbb.a $(SDLIB)/ && \
	cp openssl-prefix/src/openssl/libcrypto.so.1.0.0 $(SDLIB)/libcrypto.so && \
	cp openssl-prefix/src/openssl/libssl.so.1.0.0 $(SDLIB)/libssl.so && \
	cp zlib-prefix/src/zlib/libz.so.1.2.11 $(SDLIB)/libz.so

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

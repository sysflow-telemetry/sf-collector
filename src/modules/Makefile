include ../makefile.manifest.inc
include ../makefile.env.inc

.PHONY: all
all: init \
	 modules

.PHONY: init
init:
	git submodule update --init --recursive
	cd sysdig && git checkout $(SYSDIG_VERSION)
	cd avro && git checkout $(AVRO_VERSION)

.PHONY: modules
modules: sysdig \
         avro

.PHONY: sysdig
sysdig: 
	cd sysdig && sed -i -E "s/SYSDIG_VERSION \".+\"/SYSDIG_VERSION \"$(SYSDIG_VERSION)\"/" CMakeLists.txt && \
	mkdir -p build && cd build && \
    cmake ../. && make openssl curl b64 jq tbb zlib driver scap sinsp && \
	mkdir -p $(SDINCLUDE) && mkdir -p $(SDLIB) && \
	mkdir -p $(SDINCLUDE)/curl && cp curl-prefix/src/curl/include/curl/*.h $(SDINCLUDE)/curl && \
	mkdir -p $(SDINCLUDE)/driver && cp driver/*h $(SDINCLUDE)/driver && cp ../driver/*h $(SDINCLUDE)/driver && \
	mkdir -p $(SDINCLUDE)/json && cp ../userspace/libsinsp/third-party/jsoncpp/jsoncpp.cpp $(SDINCLUDE)/json && \
	mkdir -p $(SDINCLUDE)/json && cp ../userspace/libsinsp/third-party/jsoncpp/json/*.h $(SDINCLUDE)/json && \
	mkdir -p $(SDINCLUDE)/userspace/common && cp ../userspace/common/*.h $(SDINCLUDE)/userspace/common && \
	mkdir -p $(SDINCLUDE)/userspace/libsinsp && cp ../userspace/libsinsp/*.h $(SDINCLUDE)/userspace/libsinsp && \
	mkdir -p $(SDINCLUDE)/userspace/libscap && cp ../userspace/libscap/*.h $(SDINCLUDE)/userspace/libscap && \
	mkdir -p $(SDINCLUDE)/openssl && cp openssl-prefix/src/openssl/include/openssl/*.h $(SDINCLUDE)/openssl && \
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
	cd avro/lang/c++ && mkdir -p build && cd build && \
	cmake -DCMAKE_CXX_FLAGS_ALL_WARNINGS:STRING="-w" \
	      -DCMAKE_BUILD_TYPE=ALL_WARNINGS -G "Unix Makefiles" ../. && \
    make install

.PHONY: uninstall
uninstall:
	rm -rf $(SDINCLUDE)
	rm -rf $(SDLIB)/libsinsp.a
	rm -rf $(SDLIB)/libscap.a
	rm -rf $(SDLIB)/libb64.a
	rm -rf $(SDLIB)/libcurl.a
	rm -rf $(SDLIB)/libjq.a
	rm -rf $(SDLIB)/libtbb.a
	rm -rf $(SDLIB)/libcrypto.so
	rm -rf $(SDLIB)/libssl.so
	rm -rf $(SDLIB)/libz.so
	rm -rf /usr/local/include/avro
	rm -rf /usr/local/lib/libavro*

.PHONY: clean
clean:
	rm -rf avro/lang/c++/build sysdig/build

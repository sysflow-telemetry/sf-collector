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
modules: sysdig/package \
         avro/package

.PHONY: install
install: modules \
	sysdig/install \
	avro/install

.PHONY: sysdig/package
sysdig/package: 
	cd sysdig && sed -i -E "s/SYSDIG_VERSION \".+\"/SYSDIG_VERSION \"$(SYSDIG_VERSION)\"/" CMakeLists.txt && \
	mkdir -p build && cd build && \
    cmake ../. && make openssl curl b64 jq tbb zlib driver scap sinsp && \
	mkdir -p include && mkdir -p lib && \
	mkdir -p include/curl && cp curl-prefix/src/curl/include/curl/*.h include/curl && \
	mkdir -p include/driver && cp driver/*h include/driver && cp ../driver/*h include/driver && \
	mkdir -p include/json && cp ../userspace/libsinsp/third-party/jsoncpp/jsoncpp.cpp include/json && \
	mkdir -p include/json && cp ../userspace/libsinsp/third-party/jsoncpp/json/*.h include/json && \
	mkdir -p include/userspace/common && cp ../userspace/common/*.h include/userspace/common && \
	mkdir -p include/userspace/libsinsp && cp ../userspace/libsinsp/*.h include/userspace/libsinsp && \
	mkdir -p include/userspace/libscap && cp ../userspace/libscap/*.h include/userspace/libscap && \
	mkdir -p include/openssl && cp openssl-prefix/src/openssl/include/openssl/*.h include/openssl && \
	cp userspace/libsinsp/libsinsp.a lib/ && \
	cp userspace/libscap/libscap.a lib/ && \
	cp b64-prefix/src/b64/src/libb64.a lib/ && \
	cp curl-prefix/src/curl/lib/.libs/libcurl.a lib/ && \
	cp jq-prefix/src/jq/.libs/libjq.a lib/ && \
	cp tbb-prefix/src/tbb/build/lib_release/libtbb.a lib/ && \
	cp openssl-prefix/src/openssl/libcrypto.so.1.0.0 lib/libcrypto.so && \
	cp openssl-prefix/src/openssl/libssl.so.1.0.0 lib/libssl.so && \
	cp zlib-prefix/src/zlib/libz.so.1.2.11 lib/libz.so

.PHONY: sysdig/install
sysdig/install:	
	mkdir -p $(SDINCPREFIX) && mkdir -p $(LIBPREFIX) && \
	cd sysdig/build && cp -r include/* $(SDINCPREFIX)/ && cp -r lib/* $(LIBPREFIX)/

.PHONY: avro/package
avro/package:
	cd avro/lang/c++ && mkdir -p build && cd build && \
		cmake -DCMAKE_INSTALL_PREFIX=$(MODPREFIX) \
		-DCMAKE_CXX_FLAGS_ALL_WARNINGS:STRING="-w" \
		-DCMAKE_BUILD_TYPE=ALL_WARNINGS -G "Unix Makefiles" ../. && make && \
		cp -r ../api avro

.PHONY: avro/install
avro/install:
	cd avro/lang/c++/build && make install

.PHONY: uninstall
uninstall:
	rm -rf $(PREFIX)/libs

.PHONY: clean
clean:
	rm -rf avro/lang/c++/build sysdig/build

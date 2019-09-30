include ../makefile.manifest.inc
include ../makefile.env.inc

TARGET=sysporter
TARGET2=sysreader
CXX=g++-8
LINT=clang-tidy-9
GCCVERSIONGTEQ8 := $(shell expr `$(CXX) -dumpversion | cut -f1 -d.` \>= 8)

LIBLOCALPREFIX=../modules
SDLOCALLIBPREFIX=$(LIBLOCALPREFIX)/sysdig/build/lib
SDLOCALINCPREFIX=$(LIBLOCALPREFIX)/sysdig/build/include
AVRLOCALLIBPREFIX=$(LIBLOCALPREFIX)/avro/lang/c++/build
AVRLOCALINCPREFIX=$(LIBLOCALPREFIX)/avro/lang/c++/build

SRCS := $(wildcard *.cpp)

LIBS= $(SDLOCALLIBPREFIX)/libsinsp.a $(SDLOCALLIBPREFIX)/libscap.a $(SDLOCALLIBPREFIX)/libjq.a \
      $(SDLOCALLIBPREFIX)/libb64.a $(SDLOCALLIBPREFIX)/libcurl.a $(SDLOCALLIBPREFIX)/libtbb.a \
      -lstdc++ -lelf -lz -lrt -lanl -lssl -lcrypto -lpthread -lm -lavrocpp -llog4cxx
LIBS2 = -lstdc++ -lpthread -lm -lavrocpp -lb64

LDFLAGS= $(LIBS) -L/usr/local/lib/ -L$(LIBPREFIX)/ -L$(SDLOCALLIBPREFIX)/ -L$(AVRLOCALLIBPREFIX)/ \
	 -Wl,-rpath=/usr/local/lib -Wl,-rpath=$(LIBPREFIX) -Wl,-rpath=$(SDLOCALLIBPREFIX) -Wl,-rpath=$(AVRLOCALLIBPREFIX)
LDFLAGS2= $(LIBS2) -L/usr/local/lib/ -L$(LIBPREFIX)/ -L$(SDLOCALLIBPREFIX)/ -L$(AVRLOCALLIBPREFIX)/ \
	  -Wl,-rpath=/usr/local/lib -Wl,-rpath=$(LIBPREFIX) -Wl,-rpath=$(SDLOCALLIBPREFIX) -Wl,-rpath=$(AVRLOCALLIBPREFIX)

CFLAGS= -std=c++17 -Wall -ggdb -I.. -I../modules/sysflow/c++/ \
		-DHAS_CAPTURE -DPLATFORM_NAME=\"Linux\" -DK8S_DISABLE_THREAD \
	 	-I$(SDLOCALINCPREFIX)/ -I$(SDLOCALINCPREFIX)/curl/ -I$(SDLOCALINCPREFIX)/json/ -I$(SDLOCALINCPREFIX)/openssl/ -I$(SDLOCALINCPREFIX)/driver/ \
		-I$(SDLOCALINCPREFIX)/userspace/libsinsp/ -I$(SDLOCALINCPREFIX)/userspace/libscap/ -I$(AVRLOCALINCPREFIX)/ -I/usr/local/include/
CFLAGS2= -std=c++17 -ggdb -I.. -I../modules/sysflow/c++/ \
	 	-I$(SDLOCALINCPREFIX)/ -I$(SDLOCALINCPREFIX)/curl/ -I$(SDLOCALINCPREFIX)/json/ -I$(SDLOCALINCPREFIX)/openssl/ -I$(SDLOCALINCPREFIX)/driver/ \
		-I$(SDLOCALINCPREFIX)/userspace/libsinsp/ -I$(SDLOCALINCPREFIX)/userspace/libscap/ -I$(AVRLOCALINCPREFIX)/ -I/usr/local/include/

ifeq "$(GCCVERSIONGTEQ8)" "1"
	LIBS+= -lstdc++fs
	LIBS2+= -lstdc++fs
else
	LIBS+= -lboost_system -lboost_filesystem
	LIBS2+= -lboost_system -lboost_filesystem
	CFLAGS+= -DSYSPORTER_BOOST
	CFLAGS2+= -DSYSPORTER_BOOST
endif

.PHONY: all
all: version $(TARGET) $(TARGET2)

.PHONY: install
install: all 
	mkdir -p $(PREFIX)/bin && cp sysporter $(PREFIX)/bin && cp sysreader $(PREFIX)/bin
	mkdir -p $(PREFIX)/conf && cp avro/avsc/SysFlow.avsc $(PREFIX)/conf && cp conf/log4cxx.properties $(PREFIX)/conf

.PHONY: uninstall
uninstall: 
	rm -rf $(PREFIX)/bin
	rm -rf $(PREFIX)/conf

.PHONY: version
version:
	cp sysflow_config.h.in sysflow_config.h
	sed -i -E "s/SYSFLOW_VERSION/\"$(SYSFLOW_VERSION)\"/" sysflow_config.h
	sed -i -E "s/SYSFLOW_BUILD_NUMBER/\"$(SYSFLOW_BUILD_NUMBER)\"/" sysflow_config.h

.PHONY: $(TARGET)
$(TARGET): .main.o .MurmurHash3.o .utils.o .containercontext.o .processcontext.o .processeventprocessor.o .dataflowprocessor.o .networkflowprocessor.o .fileflowprocessor.o .fileeventprocessor.o .sysflowcontext.o .sysflowprocessor.o .sysflowwriter.o .filecontext.o
	$(CXX) $^ -o $@ $(LDFLAGS)

.PHONY: $(TARGET2)
$(TARGET2): .reader.o .MurmurHash3.o
	$(CXX) $^ -o $@ $(LDFLAGS2)

.PHONY: lint
lint: $(SRCS)
	$(LINT) -checks=* $^ -- $(CFLAGS)

.main.o: main.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.sysflowfork.o: sysflowfork.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.MurmurHash3.o: MurmurHash3.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.utils.o: utils.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.containercontext.o: containercontext.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.sysflowprocessor.o: sysflowprocessor.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.sysflowwriter.o: sysflowwriter.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.processcontext.o: processcontext.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.processeventprocessor.o: processeventprocessor.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.dataflowprocessor.o: dataflowprocessor.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.networkflowprocessor.o: networkflowprocessor.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.networkflowprocessor.lint: networkflowprocessor.cpp
	$(LINT) -checks=* $^ -- $(CFLAGS)

.fileflowprocessor.o: fileflowprocessor.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.fileeventprocessor.o: fileeventprocessor.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.sysflowcontext.o: sysflowcontext.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.filecontext.o: filecontext.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

.reader.o: reader.cpp
	$(CXX) $(CFLAGS2) -o $@ -c $^

.PHONY: clean
clean:
	rm -f .[!.]*.o *.o *.so *.a $(TARGET) $(TARGET2) sysflow_config.h

.PHONY : help
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... install"
	@echo "... uninstall"

include ../makefile.manifest.inc
include ../makefile.env.inc

TARGET=sysporter
TARGET2=sysreader
GCCVERSIONGTEQ8 := $(shell expr `g++ -dumpversion | cut -f1 -d.` \>= 8)

LIBLOCALPREFIX=../modules
SDLOCALLIBPREFIX=$(LIBLOCALPREFIX)/sysdig/build/lib
SDLOCALINCPREFIX=$(LIBLOCALPREFIX)/sysdig/build/include
AVRLOCALLIBPREFIX=$(LIBLOCALPREFIX)/avro/lang/c++/build
AVRLOCALINCPREFIX=$(LIBLOCALPREFIX)/avro/lang/c++/build

LDFLAGS= $(SDLOCALLIBPREFIX)/libsinsp.a $(SDLOCALLIBPREFIX)/libscap.a $(SDLOCALLIBPREFIX)/libjq.a \
	$(SDLOCALLIBPREFIX)/libb64.a $(SDLOCALLIBPREFIX)/libcurl.a $(SDLOCALLIBPREFIX)/libtbb.a \
	-lelf -lz -lrt -lanl -lssl -lcrypto -lpthread -lm -lavrocpp -llog4cxx \
	-L /usr/local/lib/ -L$(LIBPREFIX)/ -L$(SDLOCALLIBPREFIX)/ -L$(AVRLOCALLIBPREFIX)/ \
	-Wl,-rpath=/usr/local/lib -Wl,-rpath=$(LIBPREFIX) -Wl,-rpath=$(SDLOCALLIBPREFIX) -Wl,-rpath=$(AVRLOCALLIBPREFIX)

LDFLAGS2= -lpthread -lm -lavrocpp -lb64 \
	-L /usr/local/lib/ -L$(LIBPREFIX)/ -L$(SDLOCALLIBPREFIX)/ -L$(AVRLOCALLIBPREFIX)/ \
	-Wl,-rpath=/usr/local/lib -Wl,-rpath=$(LIBPREFIX) -Wl,-rpath=$(SDLOCALLIBPREFIX) -Wl,-rpath=$(AVRLOCALLIBPREFIX)

CFLAGS= -no-pie -std=c++17 -Wall -ggdb -O3 -ggdb -I.. -I./avro/src/ \
		-DHAS_CAPTURE -DPLATFORM_NAME=\"Linux\" -DK8S_DISABLE_THREAD \
	 	-I$(SDLOCALINCPREFIX)/ -I$(SDLOCALINCPREFIX)/curl/ -I$(SDLOCALINCPREFIX)/json/ -I$(SDLOCALINCPREFIX)/openssl/ -I$(SDLOCALINCPREFIX)/driver/ \
		-I$(SDLOCALINCPREFIX)/userspace/libsinsp/ -I$(SDLOCALINCPREFIX)/userspace/libscap/ -I$(AVRLOCALINCPREFIX)/ -I/usr/local/include/
CFLAGS2= -std=c++17 -ggdb -I.. -I./avro/src/ \
	 	-I$(SDLOCALINCPREFIX)/ -I$(SDLOCALINCPREFIX)/curl/ -I$(SDLOCALINCPREFIX)/json/ -I$(SDLOCALINCPREFIX)/openssl/ -I$(SDLOCALINCPREFIX)/driver/ \
		-I$(SDLOCALINCPREFIX)/userspace/libsinsp/ -I$(SDLOCALINCPREFIX)/userspace/libscap/ -I$(AVRLOCALINCPREFIX)/ -I/usr/local/include/

ifeq "$(GCCVERSIONGTEQ8)" "1"
	LDFLAGS+= -lstdc++fs
else
	LDFLAGS+= -lboost_filesystem
	CFLAGS+= -DSYSPORTER_BOOST
	CFLAGS2+= -DSYSPORTER_BOOST
endif

.PHONY: all
all: $(TARGET) $(TARGET2)

.PHONY: install
install: all 
	mkdir -p $(PREFIX)/bin && cp sysporter $(PREFIX)/bin && cp sysreader $(PREFIX)/bin
	mkdir -p $(PREFIX)/conf && cp avro/avsc/SysFlow.avsc $(PREFIX)/conf

.PHONY: uninstall
uninstall: 
	rm -rf $(PREFIX)/bin
	rm -rf $(PREFIX)/conf

.PHONY: $(TARGET)
$(TARGET): .main.o .MurmurHash3.o .utils.o .containercontext.o .processcontext.o .processeventprocessor.o .dataflowprocessor.o .networkflowprocessor.o .fileflowprocessor.o .fileeventprocessor.o .sysflowcontext.o .sysflowprocessor.o .sysflowwriter.o .filecontext.o
	g++ $^ -o $@ $(LDFLAGS)

.PHONY: $(TARGET2)
$(TARGET2): .reader.o .MurmurHash3.o
	g++ $^ -o $@ $(LDFLAGS2)

.main.o: main.cpp
	g++ $(CFLAGS) -o $@ -c $^

.sysflowfork.o: sysflowfork.cpp
	g++ $(CFLAGS) -o $@ -c $^

.MurmurHash3.o: MurmurHash3.cpp
	g++ $(CFLAGS) -o $@ -c $^

.utils.o: utils.cpp
	g++ $(CFLAGS) -o $@ -c $^

.containercontext.o: containercontext.cpp
	g++ $(CFLAGS) -o $@ -c $^

.sysflowprocessor.o: sysflowprocessor.cpp
	g++ $(CFLAGS) -o $@ -c $^

.sysflowwriter.o: sysflowwriter.cpp
	g++ $(CFLAGS) -o $@ -c $^

.processcontext.o: processcontext.cpp
	g++ $(CFLAGS) -o $@ -c $^

.processeventprocessor.o: processeventprocessor.cpp
	g++ $(CFLAGS) -o $@ -c $^

.dataflowprocessor.o: dataflowprocessor.cpp
	g++ $(CFLAGS) -o $@ -c $^

.networkflowprocessor.o: networkflowprocessor.cpp
	g++ $(CFLAGS) -o $@ -c $^

.fileflowprocessor.o: fileflowprocessor.cpp
	g++ $(CFLAGS) -o $@ -c $^

.fileeventprocessor.o: fileeventprocessor.cpp
	g++ $(CFLAGS) -o $@ -c $^

.sysflowcontext.o: sysflowcontext.cpp
	g++ $(CFLAGS) -o $@ -c $^

.filecontext.o: filecontext.cpp
	g++ $(CFLAGS) -o $@ -c $^

.reader.o: reader.cpp
	g++ $(CFLAGS2) -o $@ -c $^

.PHONY: clean
clean:
	rm -f .[!.]*.o *.o *.so *.a $(TARGET) $(TARGET2)

.PHONY : help
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... install"
	@echo "... uninstall"

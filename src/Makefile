include makefile.manifest.inc
include makefile.env.inc

TARGET=sysporter
TARGET2=sysreader
GCCVERSIONGTEQ8 := $(shell expr `g++ -dumpversion | cut -f1 -d.` \>= 8)

LDFLAGS= -no-pie $(SDLIB)/libsinsp.a $(SDLIB)/libscap.a $(SDLIB)/libjq.a \
		 $(SDLIB)/libb64.a $(SDLIB)/libcurl.a $(SDLIB)/libtbb.a -L$(SDLIB)/ \
		 -lelf -lz -lrt -lanl -lssl -lcrypto -lpthread -lm -lavrocpp -Wl,-rpath=$(SDLIB) -Wl,-rpath=/usr/local/lib
LDFLAGS2=-L$(SDLIB)/ -lpthread -lm -lavrocpp -lb64 -Wl,-rpath=$(SDLIB) -Wl,-rpath=/usr/local/lib

CFLAGS= -no-pie -std=c++17 -Wall -ggdb -O3 -ggdb -I$(SDINCLUDE) -I.. -I./avro/src/ \
		-DHAS_CAPTURE -DPLATFORM_NAME=\"Linux\" -DK8S_DISABLE_THREAD -I$(SDINCLUDE)/driver/ \
		-I$(SDINCLUDE)/userspace/libsinsp/ -I$(SDINCLUDE)/userspace/libscap/ -I/usr/local/include
CFLAGS2= -std=c++17 -ggdb -I./avro/src/ -I$(SDINCLUDE)/userspace/libsinsp/ -I$(SDINCLUDE)/userspace/libscap/ -I$(SDINCLUDE)

ifeq "$(GCCVERSIONGTEQ8)" "1"
	LDFLAGS+= -lstdc++fs
else
	LDFLAGS+= -lboost_filesystem
	CFLAGS+= -DSYSPORTER_BOOST
endif
 
.PHONY: install
install: all sysporter_install

.PHONY: sysporter_install
install:
	mkdir -p $(PREFIX)/bin && cp sysporter $(PREFIX)/bin && cp sysreader $(PREFIX)/bin
	mkdir -p $(PREFIX)/conf && cp avro/avsc/SysFlow.avsc $(PREFIX)/conf

.PHONY: uninstall
uninstall: 
	rm -rf $(PREFIX)
	cd modules && make uninstall

.PHONY: all
all: modules sysporter

.PHONY: sysporter
all: $(TARGET) $(TARGET2)

.PHONY: modules
modules:
	cd modules && make modules

.PHONY: avro
avro:
	cd modules && make avro

.PHONY: sysdig
sysdig:
	cd modules && make sysdig

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
	rm -f *.o *.so *.a $(TARGET) $(TARGET2)
	cd modules && make clean

.PHONY : help
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... modules"
	@echo "... avro"
	@echo "... sysdig"
	@echo "... install"

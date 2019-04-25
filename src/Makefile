#-------------------------------
# SysFlow Makefile
#-------------------------------

include makefile.manifest.inc
include makefile.env.inc

TARGET=sysporter
TARGET2=sysreader

LDFLAGS= -no-pie $(SDLIB)/libsinsp.a $(SDLIB)/libscap.a $(SDLIB)/libjq.a \
		 $(SDLIB)/libb64.a $(SDLIB)/libcurl.a $(SDLIB)/libtbb.a -L$(SDLIB)/ \
		 -lelf -lz -lrt -lanl -lssl -lcrypto -lpthread -lm -lavrocpp -lstdc++fs
LDFLAGS2=-L$(SDLIB)/ -lpthread -lm -lavrocpp -lb64 -lstdc++fs

CFLAGS= -no-pie -std=c++17 -Wall -ggdb -O3 -ggdb -I$(SDINCLUDE) -I.. -I./avro/src/ \
		-DHAS_CAPTURE -DPLATFORM_NAME=\"Linux\" -DK8S_DISABLE_THREAD -I$(SDINCLUDE)/driver/ \
		-I$(SDINCLUDE)/userspace/libsinsp/ -I$(SDINCLUDE)/userspace/libscap/ -I/usr/local/include
CFLAGS2= -std=c++17 -ggdb -I./avro/src/ -I$(SDINCLUDE)/userspace/libsinsp/ -I$(SDINCLUDE)/userspace/libscap/ -I$(SDINCLUDE)

.PHONY: install
install: all
	#mkdir -p $(PREFIX)/bin
	#cp TARGET $(PREFIX)/bin/
	#cp TARGET $(PREFIX)/bin/

.PHONY: uninstall
uninstall: 
	@echo "not implemented"

.PHONY: all
all: modules $(TARGET) $(TARGET2)

.PHONY: modules
modules:
	cd modules && make

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

.PHONY : help
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... modules"
	@echo "... avro"
	@echo "... sysdig"
	@echo "... install"

.PHONY: clean
LDFLAGS= ../libs/sysdig/lib/libsinsp.a ../libs/sysdig/lib/libscap.a -lelf -L../libs/sysdig/lib/ -lz ../libs/sysdig/lib/libjq.a ../libs/sysdig/lib/libb64.a ../libs/sysdig/lib/libcurl.a -lrt  -lanl -L../libs/sysdig/lib -lssl -lcrypto -lpthread -lm -lavrocpp ../libs/sysdig/lib/libtbb.a -lboost_filesystem
LDFLAGS2=-L./../libs/sysdig/lib/ -lpthread -lm -lavrocpp -lb64

CFLAGS= -std=c++17 -Wall -ggdb -O3 -ggdb -I./../libs/sysdig/include -I.. -I./avro/src/ -DHAS_CAPTURE -DPLATFORM_NAME=\"Linux\" -DK8S_DISABLE_THREAD -I./../libs/sysdig/include/driver/ -I./../libs/sysdig/include/userspace/libsinsp/ -I./../libs/sysdig/include/userspace/libscap/ -I/usr/local/include
CFLAGS2= -std=c++17 -ggdb -I./avro/src/  -I./../libs/sysdig/include/userspace/libsinsp/ -I./../libs/sysdig/include/userspace/libscap/ -I./../libs/sysdig/include

TARGET=sysporter
TARGET2=sysreader
TARGET3=sysporterf

all: $(TARGET) $(TARGET2)

$(TARGET): main.o MurmurHash3.o utils.o containercontext.o processcontext.o processeventprocessor.o dataflowprocessor.o networkflowprocessor.o fileflowprocessor.o fileeventprocessor.o sysflowcontext.o sysflowprocessor.o sysflowwriter.o filecontext.o
	g++ $^ -o $@ $(LDFLAGS)

$(TARGET2): reader.o MurmurHash3.o
	g++ $^ -o $@ $(LDFLAGS2)

main.o: main.cpp
	g++ $(CFLAGS) -o $@ -c $^

sysflowfork.o: sysflowfork.cpp
	g++ $(CFLAGS) -o $@ -c $^

MurmurHash3.o: MurmurHash3.cpp
	g++ $(CFLAGS) -o $@ -c $^

utils.o: utils.cpp
	g++ $(CFLAGS) -o $@ -c $^

containercontext.o: containercontext.cpp
	g++ $(CFLAGS) -o $@ -c $^

sysflowprocessor.o: sysflowprocessor.cpp
	g++ $(CFLAGS) -o $@ -c $^

sysflowwriter.o: sysflowwriter.cpp
	g++ $(CFLAGS) -o $@ -c $^

processcontext.o: processcontext.cpp
	g++ $(CFLAGS) -o $@ -c $^

processeventprocessor.o: processeventprocessor.cpp
	g++ $(CFLAGS) -o $@ -c $^

dataflowprocessor.o: dataflowprocessor.cpp
	g++ $(CFLAGS) -o $@ -c $^

networkflowprocessor.o: networkflowprocessor.cpp
	g++ $(CFLAGS) -o $@ -c $^

fileflowprocessor.o: fileflowprocessor.cpp
	g++ $(CFLAGS) -o $@ -c $^

fileeventprocessor.o: fileeventprocessor.cpp
	g++ $(CFLAGS) -o $@ -c $^

sysflowcontext.o: sysflowcontext.cpp
	g++ $(CFLAGS) -o $@ -c $^

filecontext.o: filecontext.cpp
	g++ $(CFLAGS) -o $@ -c $^

reader.o: reader.cpp
	g++ $(CFLAGS2) -o $@ -c $^
clean:
	rm -f *.o *.so *.a $(TARGET) $(TARGET2)

.PHONY: clean
#cd /home/terylt/workspace/cscap/sysdig-0.23.0/build/userspace/sysdig && /usr/bin/c++   -DHAS_CAPTURE -DK8S_DISABLE_THREAD -DPLATFORM_NAME=\"Linux\" -I/home/terylt/workspace/cscap/sysdig-0.23.0/userspace/libsinsp/third-party/jsoncpp -I/home/terylt/workspace/cscap/sysdig-0.23.0/build/curl-prefix/src/curl/include -I/home/terylt/workspace/cscap/sysdig-0.23.0/build/ncurses-prefix/src/ncurses/include -I/home/terylt/workspace/cscap/sysdig-0.23.0/common -I/home/terylt/workspace/cscap/sysdig-0.23.0/userspace/libscap -I/home/terylt/workspace/cscap/sysdig-0.23.0/userspace/libsinsp -I/home/terylt/workspace/cscap/sysdig-0.23.0/build/userspace/sysdig -I/home/terylt/workspace/cscap/sysdig-0.23.0/userspace/sysdig/.  -Wall -ggdb -O3 -DNDEBUG -fPIE   -std=gnu++11 -o CMakeFiles/sysdig.dir/sysdig.cpp.o -c /home/terylt/workspace/cscap/sysdig-0.23.0/userspace/sysdig/sysdig.cpp
#LDFLAGS=-rdynamic  -L../../sysdig-0.23.0/build/userspace/libsinsp/ -lsinsp -L../../sysdig-0.23.0/build/userspace/libscap/ -lscap -lelf -L../../sysdig-0.23.0/build/zlib-prefix/src/zlib/ -lz -L../../sysdig-0.23.0/build/jq-prefix/src/jq/.libs/ -ljq -L../../sysdig-0.23.0/build/b64-prefix/src/b64/src/ -lb64 -L../../sysdig-0.23.0/build/curl-prefix/src/curl/lib/.libs/ -lcurl -lrt -lanl -L../../sysdig-0.23.0/build/openssl-prefix/src/openssl/target/lib/ -lssl -L../../sysdig-0.23.0/build/openssl-prefix/src/openssl/target/lib/ -lcrypto -L../../sysdig-0.23.0/build/luajit-prefix/src/luajit/src/ -lluajit -ldl -lpthread -lstdc++ -lm -lavrocpp
#LDFLAGS=-rdynamic  -L../../../sysdig-0.23.0/build/userspace/libsinsp/ -lsinsp -L../../../sysdig-0.23.0/build/userspace/libscap/ -lscap -lelf -L../../../sysdig-0.23.0/build/zlib-prefix/src/zlib/ -lz -L../../../sysdig-0.23.0/build/jq-prefix/src/jq/.libs/ -ljq  -L../../../sysdig-0.23.0/build/b64-prefix/src/b64/src/ -lb64 -L../../../sysdig-0.23.0/build/curl-prefix/src/curl/lib/.libs/ -lcurl -lrt  -lanl -L../../../sysdig-0.23.0/build/openssl-prefix/src/openssl/target/lib/ -lssl -L../../../sysdig-0.23.0/build/openssl-prefix/src/openssl/target/lib/ -lcrypto -L../../../sysdig-0.23.0/build/luajit-prefix/src/luajit/src/ -lpthread -lm -lavrocpp 
LDFLAGS=-rdynamic  -L./../lib-sysdig/ -lsinsp -lscap -lelf -lz -ljq -lb64 -lcurl -lrt  -lanl -lssl -lcrypto -lpthread -lm -lavrocpp -ltbb
LDFLAGS2=-lpthread -lm -lavrocpp 
#CFLAGS=-std=c++11 -ggdb -I../../sysdig-0.23.0/userspace/libsinsp/ -I../../sysdig-0.23.0/userspace/libscap/ -I../../sysdig-0.23.0/userspace/libsinsp/third-party/jsoncpp/ -I.. -I./avro_union/src/
#CFLAGS= -std=c++11 -Wall -ggdb -O3 -DNDEBUG -ggdb -I../../sysdig-0.23.0/userspace/libsinsp/ -I../../sysdig-0.23.0/userspace/libscap/ -I../../sysdig-0.23.0/userspace/libsinsp/third-party/jsoncpp/ -I.. -I./avro_union/src/ -DHAS_CAPTURE -DPLATFORM_NAME=\"Linux\" -I../../sysdig-0.23.0/build/curl-prefix/src/curl/include/
CFLAGS= -std=c++11 -Wall -ggdb -O3 -ggdb -I./../include-sysdig -I.. -I./avro_union/src/ -DHAS_CAPTURE -DPLATFORM_NAME=\"Linux\" -DK8S_DISABLE_THREAD -fPIE -I./../include-sysdig/driver/ -I./../include-sysdig/userspace/libsinsp/ -I./../include-sysdig/userspace/libscap/
CFLAGS2= -std=c++11 -O3 -I./avro_union/src/  -I./../include-sysdig/userspace/libsinsp/ -I./../include-sysdig/userspace/libscap/  -I./../include-sysdig
#CFLAGS= -std=c++11 -ggdb -ggdb -I../../sysdig-0.23.0/userspace/libsinsp/ -I../../sysdig-0.23.0/userspace/libscap/ -I../../sysdig-0.23.0/userspace/libsinsp/third-party/jsoncpp/ -I.. -I./avro_union/src/ -I../../sysdig-0.23.0/build/curl-prefix/src/curl/include/ -DHAS_CAPTURE
TARGET=sysporter
TARGET2=sysreader
TARGET3=sysporterf

all: $(TARGET) $(TARGET2)

$(TARGET): main.o MurmurHash3.o utils.o containercontext.o processcontext.o processflowprocessor.o dataflowprocessor.o networkflowprocessor.o sysflowcontext.o sysflowprocessor.o sysflowwriter.o filecontext.o
	g++ $^ -o $@ $(LDFLAGS)

#$(TARGET3): sysflowfork.o MurmurHash3.o
#	g++ $^ -o $@ $(LDFLAGS)

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

processflowprocessor.o: processflowprocessor.cpp
	g++ $(CFLAGS) -o $@ -c $^

dataflowprocessor.o: dataflowprocessor.cpp
	g++ $(CFLAGS) -o $@ -c $^

networkflowprocessor.o: networkflowprocessor.cpp
	g++ $(CFLAGS) -o $@ -c $^

sysflowcontext.o: sysflowcontext.cpp
	g++ $(CFLAGS) -o $@ -c $^

filecontext.o: filecontext.cpp
	g++ $(CFLAGS) -o $@ -c $^

reader.o: reader.cpp
	g++ $(CFLAGS2) -o $@ -c $^
clean:
	rm -f *.o *.so *.a $(TARGET) $(TARGET2)

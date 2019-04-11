### This sample contains two basic tests:

* full capture: records the container scap from launch to tests (`./run`, `./test` command sequence)
* cold start capture: records the already started container scap from tests (`./test` command sequence)

### To install ab:
```
sudo apt-get install apache2-utils
```

### To build and run samples:
```
./build
./run
```

### To generate workload:
```
./test
```

### To capture scap:
```
./capture <container name> <scap file name>
```

# Installation and Usage

## Installing sysporter

### Cloning source

The sf-collector project has been tested primarily on Ubuntu 16.04 and 18.04.  The project will be tested on other flavors of UNIX in the future. This document describes how to build and run the application both inside a docker container and on a linux host. Building and running the application inside a docker container is the easiest way to start. 

To build the project, first pull down the source code, with submodules:
```
git clone git@github.com:sysflow-telemetry/sf-collector.git --recursive
```

To checkout submodules on an already cloned repo:
```
git submodule update --init --recursive
```

### Building a Docker container

To build a docker container: 
```
cd sf-collector
make -C modules init
export TRAVIS_BUILD_NUMBER=10
docker build --cache-from sysporter:deps --target deps -t sysporter:deps .
docker build --build-arg TRAVIS_BUILD_NUMBER --cache-from sysporter:deps --cache-from sysporter:builder --target builder -t sysporter:builder .
docker build --cache-from sysporter:deps --cache-from sysporter:builder --cache-from sysporter:runtime --target runtime -t sysporter:runtime .
docker build --cache-from sysporter:deps --cache-from sysporter:builder --cache-from sysporter:runtime --cache-from sysporter:testing --target testing -t sysporter:testing .
``` 

The container is built in stages to enable caching of the intermediate steps of the build.  As a result, the docker container of interest is: `sysporter:runtime`

### Building directly on a host

First, the dependencies to build Sysdig and the Avro libraries must be available:
```
apt-utils build-essential libncurses5-dev libncursesw5-dev cmake libboost-all-dev flex bison g++ wget libelf-dev
```

Then the dependencies for sysporter need to be installed.  Note that it is recommended that sysporter is built with g++8 or above. When building with older g++ versions, one must install the libboost filesystem library that supports the `weakly_canonical` API:
```
apt-utils make libboost-all-dev g++-8 libelf-dev liblog4cxx-dev libapr1 libaprutil1 libsparsehash-dev
```

```
ln -s /usr/bin/g++-8 /usr/bin/g++
make install
```

## Running sysporter

### Running sysporter from the command line 

Sysporter has the following options:
```
Usage: sysporter [-h] [-G &lt;interval&gt;] [-s &lt;schema file&gt;] [-c] [-e &lt;exporterID&gt;] [-r &lt;scap file&gt;] [-l &lt;log conf file&gt;] -w &lt;file name/dir&gt;</code>

Options:
  -h                            Show this help message
  -w file name/dir (required)   The file or directory to which sysflow records are written. If a directory is specified     (using a trailing slash), file name will be an epoch timestamp. If -G is specified, then the file name specified will have an epoch timestamp appended to it
  -e exporterID                 A globally unique ID representing the host or VM being monitored which is stored in the sysflow dumpfile header. If -e not set, the hostname of the CURRENT machine is used, which may not be accurate for reading offline scap files
  -G interval (in secs)         Rotates the dumpfile specified in -w every interval seconds and appends epoch timestamp to file name
  -r scap file                  The scap file to be read and dumped as sysflow format at the file specified by -w. If this option is not specified, a live capture is assumed
  -s schema file                The sysflow avro schema file (.avsc) used for schema validation (default: /usr/local/sysflow/conf/SysFlow.avsc)
  -f filter                     Sysdig style filtering string to filter scap. Must be surrounded by quotes
  -c                            Simple, fast filter to allow only container-related events to be dumped
  -l log conf file              Location of log4cxx properties configuration file. (default: /usr/local/sysflow/conf/log4cxx.properties). Properties file follows log4j format. See conf directory in github for example.  Setting log level to debug is extremely verbose
  -p cri-o path                 The path to the cri-o domain socket (BRANCH 0.26.4 only)
  -t cri-o timeout              The amount of time in ms to wait for cri-o socket to respond (BRANCH 0.26.4 only)
  -v                            Prints the version of sysporter and exits
``` 

### Example usage

Convert Sysdig scap file to SysFlow file with an export id. The output will be written to `output.sf`.  Note that sysporter must be run with root privilege:

```
sysporter -r input.scap -w ./output.sf  -e host
```

Trace a system live, and output SysFlow to files in a directory which are rotated every 30 seconds. The file name will be an epoch timestamp of when the file was initially written.  Note that the trailing slash _must be present_. The example filter ensures that only SysFlow from containers is generated.

```
sysporter -G 30 -w ./output/ -e host -f "container.type!=host and container.type=docker"
```

Trace a system live, and output SysFlow to files in a directory which are rotated every 30 seconds. The file name will be an `output.<epoch timestamp>` where the timestamp is of when the file was initially written. The example filter ensures that only SysFlow from containers is generated.

```
sysporter -G 30 -w ./output/output -e host -f "container.type!=host and container.type=docker" </code>`
```

### Running sysporter from a Docker container

The easiest way to run sysporter is from a Docker container, with host mount for the files.  The following command demonstrates how to run sysporter where files are exported to `/mnt/data` on the host.

```
mkdir -p /mnt/data
docker pull sysflowtelemetry/sf-collector

docker run -d --privileged --name sf-collector  -v /var/run/docker.sock:/host/var/run/docker.sock \
             -v /dev:/host/dev -v /proc:/host/proc:ro -v /boot:/host/boot:ro -v /lib/modules:/host/lib/modules:ro \
             -v /usr:/host/usr:ro -v /mnt/data:/mnt/data \
             -e INTERVAL=300 \
             -e NODE_NAME="${NODE}" \
             -e OUTPUT=/mnt/data    \
             -e FILTER="-f \"container.type!=host and container.type=docker and container.name!=sf-collector and not (container.name contains sf-exporter)\"" \
             --rm -i -t  floripa.sl.cloud9.ibm.com/sf-collector
```

where INTERVAL corresponds to the value of `-G`, NODE\_NAME corresponds to the value of `-e`, OUTPUT corresponds to the value of `-w`, and FILTER represents both the flag (`-f`) and the actual filter of sysporter (see above).

### CRI-O support

The sf-collector project currently supports docker and kubernetes deployments (using the helm charts provided in sf-deployments). Container runtimes based on CRI-O is planned for futures releases of the collector.

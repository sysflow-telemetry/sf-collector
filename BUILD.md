# Installation and Usage

## Installing the collector

### Cloning source

The sf-collector project has been tested primarily on Ubuntu 16.04 and 18.04.  The project will be tested on other flavors of UNIX in the future. This document describes how to build and run the application both inside a docker container and on a linux host. Building and running the application inside a docker container is the easiest way to start. For convenience, skip the build step and pull pre-built images directly from [Docker Hub](https://hub.docker.com/r/sysflowtelemetry/sf-collector).

To build the project, first pull down the source code, with submodules:
```
git clone --recursive git@github.com:sysflow-telemetry/sf-collector.git 
```

To checkout submodules on an already cloned repo:
```
git submodule update --init --recursive
```

### Building as Docker container

To build as docker container: 
```
cd sf-collector
make -C modules init
docker build --target runtime -t sf-collector .
``` 

The container is built in stages to enable caching of the intermediate steps of the build and reduce final image sizes. 

### Building directly on a host

First, install required dependencies:
```
apt install patch base-files binutils bzip2 libdpkg-perl perl make xz-utils libncurses5-dev libncursesw5-dev cmake libboost-all-dev g++  flex bison wget libelf-dev liblog4cxx-dev libapr1 libaprutil1 libsparsehash-dev
```

To build the collector:
```
cd sf-collector
make install
```

## Running the collector

### Running the collector from the command line 

The collector has the following options:
```
Usage: sysporter [options] -w <file name/dir>

Options:
  -h                            Show this help message and exit
  -w file name/dir (required)   The file or directory to which sysflow records are written. If a directory is specified     (using a trailing slash), file name will be an epoch timestamp. If -G is specified, then the file name specified will have an epoch timestamp appended to it
  -e exporterID                 A globally unique ID representing the host or VM being monitored which is stored in the sysflow dumpfile header. If -e not set, the hostname of the CURRENT machine is used, which may not be accurate for reading offline scap files
  -G interval (in secs)         Rotates the dumpfile specified in -w every interval seconds and appends epoch timestamp to file name
  -r scap file                  The scap file to be read and dumped as sysflow format at the file specified by -w. If this option is not specified, a live capture is assumed
  -s schema file                The sysflow avro schema file (.avsc) used for schema validation (default: /usr/local/sysflow/conf/SysFlow.avsc)
  -f filter                     Sysdig style filtering string to filter scap. Must be surrounded by quotes
  -c                            Simple, fast filter to allow only container-related events to be dumped
  -p cri-o path                 The path to the cri-o domain socket
  -t cri-o timeout              The amount of time in ms to wait for cri-o socket to respond
  -u domain socket file         Outputs SysFlow to a unix domain socket rather than to a file
  -v                            Print version information and exit
``` 

### Example usage

Convert Sysdig scap file to SysFlow file with an export id. The output will be written to `output.sf`.  Note that the collector must be run with root privilege:

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

### Running the collector from a Docker container

The easiest way to run the SysFlow collector is from a Docker container, with host mount for the output trace files. The following command shows how to run sf-collector with trace files exported to `/mnt/data` on the host.

```
docker run -d --privileged --name sf-collector \
	     -v /var/run/docker.sock:/host/var/run/docker.sock \
	     -v /dev:/host/dev -v /proc:/host/proc:ro \
	     -v /boot:/host/boot:ro -v /lib/modules:/host/lib/modules:ro \
             -v /usr:/host/usr:ro -v /mnt/data:/mnt/data \
             -e INTERVAL=60 \
             -e EXPORTER_ID=${HOSTNAME} \
             -e OUTPUT=/mnt/data/    \
             -e FILTER="container.name!=sf-collector and container.name!=sf-exporter" \
             --rm sysflow-telemetry/sf-collector
```
where INTERVAL denotes the time in seconds before a new trace file is generated, EXPORTER\_ID sets the exporter name, OUTPUT is the directory in which trace files are written, and FILTER is the filter expression used to filter collected events. Note: append `container.type!=host` to FILTER expression to filter host events. 

### CRI-O support

The sf-collector project currently supports docker and kubernetes deployments (using the helm charts provided in sf-deployments). Container runtimes based on CRI-O is planned for futures releases of the collector.

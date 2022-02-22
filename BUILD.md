# Installation and Usage

## Installation

### Cloning sources

This document describes how to build and run the SysFlow Collector both inside a docker container and on a linux host. Binary packages are also available in the [deployments repository](https://github.com/sysflow-telemetry/sf-deployments). Building and running the application inside a docker container is the easiest way to start. For convenience, skip the build step and pull pre-built images directly from [Docker Hub](https://hub.docker.com/r/sysflowtelemetry/sf-collector).

To build the project, first clone the repository:

```bash
git clone --recursive https://github.com/sysflow-telemetry/sf-collector.git 
```

### Building as Docker container

To build as docker container, run:

```bash
make docker-build
```

### Building directly on a host

First, install required dependencies.

On Rhel-based hosts:

```bash
scripts/installUBIDependency.sh
```

On Debian-based hosts:

```bash
apt install -y patch base-files binutils bzip2 libdpkg-perl perl make xz-utils libncurses5-dev libncursesw5-dev cmake libboost-all-dev g++ flex bison wget libelf-dev liblog4cxx-dev libapr1 libaprutil1 libsparsehash-dev libsnappy-dev libgoogle-glog-dev libjsoncpp-dev
```

To build the collector:

```bash
make
```

## Running

### Command line usage

To list command line options for the collector, run:

```bash
sysporter -h
```

#### Examples

To convert `scap` files to SysFlow traces with an export id. The output will be written to `output.sf`.

```bash
sysporter -r input.scap -w ./output.sf  -e host
```

Trace a system live, and output SysFlow to files in a directory which are rotated every 30 seconds. The file name will be an epoch timestamp of when the file was initially written. Note that the trailing slash _must be present_. The example filter ensures that only SysFlow from containers is generated.

```bash
sysporter -G 30 -w ./output/ -e host -f "container.type!=host and container.type=docker"
```

Trace a system live, and output SysFlow to files in a directory which are rotated every 30 seconds. The file name will be an `output.<epoch timestamp>` where the timestamp is of when the file was initially written. The example filter ensures that only SysFlow from containers is generated.

```bash
sysporter -G 30 -w ./output/output -e host -f "container.type!=host and container.type=docker" </code>`
```

### Docker usage

The easiest way to run the SysFlow collector is from a Docker container, with host mount for the output trace files. The following command shows how to run sf-collector with trace files exported to `/mnt/data` on the host.

```bash
docker run -d --privileged --name sf-collector \
	     -v /var/run/docker.sock:/host/var/run/docker.sock \
	     -v /dev:/host/dev -v /proc:/host/proc:ro \
	     -v /boot:/host/boot:ro -v /lib/modules:/host/lib/modules:ro \
             -v /usr:/host/usr:ro -v /mnt/data:/mnt/data \
             -e INTERVAL=60 \
             -e EXPORTER_ID=${HOSTNAME} \
             -e OUTPUT=/mnt/data/    \
             -e FILTER="container.name!=sf-collector and container.name!=sf-exporter" \
             --rm sysflowtelemetry/sf-collector
```

where INTERVAL denotes the time in seconds before a new trace file is generated, EXPORTER\_ID sets the exporter name, OUTPUT is the directory in which trace files are written, and FILTER is the filter expression used to filter collected events.

>---
> **Note** append `container.type!=host` to FILTER expression to filter host events.
>
>---

The key setting in the collector configuration is the `FILTER` variable. The collector is built atop the [Falco libs](https://github.com/falcosecurity/libs/) and it uses Falco's filtering mechanism described [here](https://falco.org/docs/rules/supported-fields/). It supports filtering on specific containers, processes, operations, etc. One of the most powerful filters is the `container.type!=host` filter, which limits collection only to container monitoring. If you want to monitor the entire host, simply remove the `container.type` operation from the filter.

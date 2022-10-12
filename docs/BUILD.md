## Build

### Cloning sources

This document describes how to build libSysFlow and run the SysFlow Collector both inside a docker container and on a linux host. Binary packages are also available in the [deployments repository](https://github.com/sysflow-telemetry/sf-deployments). Building and running the application inside a docker container is the easiest way to start. For convenience, skip the build step and pull pre-built images directly from [Docker Hub](https://hub.docker.com/r/sysflowtelemetry/sf-collector).

To build the project, first clone the repository:

```bash
git clone --recursive https://github.com/sysflow-telemetry/sf-collector.git
```

### Manifest

The [manifest](https://github.com/sysflow-telemetry/sf-collector/makefile.manifest.inc) file contains the metadata and versions of dependencies used to build libSysFlow and the Collector. It can be modified to customize the build to specifc package versions.

### Building using Docker

This is the simplest way of reliably building the collector. To build using docker, run:

```bash
make build
```

>---
> **Note** A musl build can be triggered using the `build/musl` target instead.
>
>---


If this is your first time building the collector, run the build task in the background and go grab a coffee :) If you have cores to spare, the build time can be reduced by setting concurrent make jobs. For example,

```bash
make MAKE_JOBS=8 build
```

During the initial build, a number of base images are created. These are only needed once per dependency version set. Pre-built versions of these images are also available in [Docker Hub](https://hub.docker.com/u/sysflowtelemetry) and [GHCR](https://github.com/orgs/sysflow-telemetry/packages?repo_name=sf-collector).

| **Image** | **Tag** | **Description** | **Dockerfile** |
|---|---|---|---|
| ghcr.io/sysflow-telemetry/ubi | base-<FALCO_LIBS_VERSION>-<FALCO_VERSION>-<UBI_VERSION> | A UBI base image containing the build dependencies for Falco and libSysFlow | [Dockerfile.ubi.amd64](https://github.com/sysflow-telemetry/sf-collector/Dockerfile.ubi.amd64)
|  | mods-<FALCO_LIBS_VERSION>-<FALCO_VERSION>-<UBI_VERSION> | A UBI base image containing the pre-installed Falco Libs and tools for building libSysFlow | [Dockerfile.ubi.amd64](https://github.com/sysflow-telemetry/sf-collector/Dockerfile.ubi.amd64)
|  | driver-<FALCO_LIBS_VERSION>-<FALCO_VERSION>-<UBI_VERSION> | A UBI base image containing the Falco driver loader and container entrypoint for creating the SysFlow Collector released image | [Dockerfile.driver.amd64](https://github.com/sysflow-telemetry/sf-collector/Dockerfile.driver.amd64)
| ghcr.io/sysflow-telemetry/sf-collector | libs-<SYSFLOW_VERSION> | A UBI base image containing libSysFlow | [Dockerfile](https://github.com/sysflow-telemetry/sf-collector/Dockerfile)
|  | collector-<SYSFLOW_VERSION> | A UBI base image containing the SysFlow Collector | [Dockerfile](https://github.com/sysflow-telemetry/sf-collector/Dockerfile)
|  | <SYSFLOW_VERSION> | The SysFlow Collector image | [Dockerfile](https://github.com/sysflow-telemetry/sf-collector/Dockerfile)

If building using musl, the following images are created instead.

| **Image** | **Tag** | **Description** | **Dockerfile** |
|---|---|---|---|
| ghcr.io/sysflow-telemetry/alpine | base-<FALCO_LIBS_VERSION>-<FALCO_VERSION>-<ALPINE_VERSION> | An Alpine base image containing the musl build dependencies for Falco and libSysFlow | [Dockerfile.alpine.amd64](https://github.com/sysflow-telemetry/sf-collector/Dockerfile.alpine.amd64)
|  | mods-<FALCO_LIBS_VERSION>-<FALCO_VERSION>-<ALPINE_VERSION> | An Alpine base image containing the pre-installed Falco Libs and tools for building a musl-based libSysFlow | [Dockerfile.alpine.amd64](https://github.com/sysflow-telemetry/sf-collector/Dockerfile.alpine.amd64)
| ghcr.io/sysflow-telemetry/ubi | driver-<FALCO_LIBS_VERSION>-<FALCO_VERSION>-<UBI_VERSION> | A UBI base image containing the Falco driver loader and container entrypoint for creating the SysFlow Collector released image | [Dockerfile.driver.amd64](https://github.com/sysflow-telemetry/sf-collector/Dockerfile.driver.amd64)
| ghcr.io/sysflow-telemetry/sf-collector-musl | libs-<SYSFLOW_VERSION> | An Alpine base image containing musl-based libSysFlow | [Dockerfile.musl](https://github.com/sysflow-telemetry/sf-collector/Dockerfile.musl)
|  | collector-<SYSFLOW_VERSION> | An Alpine base image containing the musl-based SysFlow Collector | [Dockerfile.musl](https://github.com/sysflow-telemetry/sf-collector/Dockerfile.musl)
|  | <SYSFLOW_VERSION> | The musl-based SysFlow Collector image | [Dockerfile.musl](https://github.com/sysflow-telemetry/sf-collector/Dockerfile.musl)

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

### Binary Packaging

You can easily package libSysFlow and the Collector using `cpack`. The deb, rpm, and tgz packages are generated in the `sctrips/cpack` directory.

To package libSysFlow headers and static libraries, run:

```bash
make build
make package-libs
```

To package a musl-based libSysFlow headers and static libraries, run:

```bash
make build/musl
make package-libs/musl
```

To package the SysFlow collector and its systemd service specification, run:

```bash
make build/musl
make package
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
			-v /dev:/host/dev \
			-v /proc:/host/proc:ro \
			-v /boot:/host/boot:ro \
			-v /lib/modules:/host/lib/modules:ro \
            -v /usr:/host/usr:ro \
			-v /etc/:/host/etc:ro \
			-v /var/lib/:/host/var/lib:ro \
			-v /mnt/data:/mnt/data \
            -e INTERVAL=60 \
            -e EXPORTER_ID=${HOSTNAME} \
            -e OUTPUT=/mnt/data/    \
            -e FILTER="container.name!=sf-collector and container.name!=sf-processor and container.name!=sf-exporter" \
            --rm sysflowtelemetry/sf-collector
```

where `INTERVAL` denotes the time in seconds before a new trace file is generated, `EXPORTER_ID` sets the exporter name, `OUTPUT` is the directory in which trace files are written, and `FILTER` is the filter expression used to filter collected events. The collector can also be setup to stream events through a unix domain socket (see [sf-deployments](https://github.com/sysflow-telemetry/sf-deployments) for other deployment configurations).

>---
> **Note** append `container.type!=host` to FILTER expression to filter host events.
>
>---

The key setting in the collector configuration is the `FILTER` variable. The collector is built atop the [Falco libs](https://github.com/falcosecurity/libs/) and it uses Falco's filtering mechanism described [here](https://falco.org/docs/rules/supported-fields/). It supports filtering on specific containers, processes, operations, etc. One of the most powerful filters is the `container.type!=host` filter, which limits collection only to container monitoring. If you want to monitor the entire host, simply remove the `container.type` operation from the filter.

## Event rate optimization

The following environment variables can be set to reduce the number of events generated by the collector:

* Drop mode (`ENABLE_DROP_MODE`=1): removes syscalls inside the kernel before they are passed up to the collector, resulting in much better performance, less spilled events, but does remove mmaps from output.
* Process flows (`ENABLE_PROC_FLOW`=1): enables the creation of process flows, aggregating thread events.
* File only (`FILE_ONLY`=1): filters out any descriptor that is not a file, including unix sockets and pipes
* File read mode (`FILE_READ_MODE`=1): sets mode for file reads. `0` enables recording all file reads as flows. `1` disables all file reads. `2` disables recording file reads to noisy directories: "/proc/", "/dev/", "/sys/", "//sys/", "/lib/",  "/lib64/", "/usr/lib/", "/usr/lib64/".

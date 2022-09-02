# LibSysFlow

LibSysFlow is a library for creating SysFlow consumers. It defines a concise API and export first-class SysFlow data types for consumers to transparently process SysFlow records and manage access to the underlying Falco libs and driver.

The main interface accepts a config object where a callback function can be set to process SysFlow records. The config option sets optimal defaults that can be customized by the consumer. The library is packaged as a static (.a) library and distributed as an rpm/deb/tgz artifact with sf-collector releases (both glibc and musl-c flavors are available).

Additionally, libsysflow performs the checks to verify that the Falco driver is loaded, and output an exception otherwise. Consumers load the Falco libs driver prior to running their main entrypoint, following the typical entrypoint recipe/script used by Falco and the SysFlow Collector.

## Basic Usage

Below is a minumum example of a SysFlow consumer that uses LibSysFlow. A more complete example can be found [here](https://github.com/sysflow-telemetry/sf-collector/tree/dev/examples/callback.cpp). The SysFlow Collector also uses LibSysFlow and serves as a [reference implementation](https://github.com/sysflow-telemetry/sf-collector/tree/dev/src/collector) for library consumers.

```cpp
// consumer-defined callback function
void process_sysflow(sysflow::SFHeader* header, sysflow::Container* cont, sysflow::Process* proc, sysflow::File* f1, sysflow::File* f2, sysflow::SysFlow* rec) {
    // your switch block here
}

// example consumer
int main(int argc, char **argv) {
    SysFlowConfig* config = sysflowlibscpp::InitializeSysFlowConfig();
    config->callback = process_sysflow;
    sysflowlibscpp::SysFlowDriver *driver = new sysflowlibscpp::SysFlowDriver(config);
    driver->run();
}
```

## Installation

Binary packages (deb, rpm, tgz) for glibc- and muslc-based build pipelines are available in the collector's [release assets](https://github.com/sysflow-telemetry/sf-collector/releases) (since release `$VERSION` >=0.5.0). This is going to install libSysFlow headers and the static libraries (.a) needed to link your application.

### Debian

Install build pre-requisites:

```bash
apt install -y make wget g++ libboost-iostreams-dev flex bison gawk libsparsehash-dev
```

Download the libSysFlow package:

```bash
wget https://github.com/sysflow-telemetry/sf-collector/releases/download/$VERSION/libsysflow-$VERSION-x86_64.deb
```

Install the libSysFlow package:

```bash
dpkg -i libsysflow-$VERSION-x86_64.deb
```

>---
> **Note** A deb package for musl builds is also available.
>
>---

### RPM

Install pre-requisites (Instructions for Rhel8 below):

```bash
subscription-manager repos --enable="codeready-builder-for-rhel-8-$(/bin/arch)-rpms"
dnf -y update
dnf -y install make wget gcc gcc-c++ glibc-static libstdc++-static flex bison boost-static sparsehash-devel
```

Download the libSysFlow package:

```bash
wget https://github.com/sysflow-telemetry/sf-collector/releases/download/$VERSION/libsysflow-$VERSION-x86_64.rpm
```

Install the libSysFlow package:

```bash
rmp -i libsysflow-$VERSION-x86_64.rpm sfprocessor-$VERSION-x86_64.rpm
```

>---
> **Note** An rpm package for musl builds is also available.
>
>---

### Alpine (musl)

Install pre-requisites:

```bash
apk add make g++ boost-dev boost-static flex bison gawk sparsehash
```

Download the libSysFlow package:

```bash
wget https://github.com/sysflow-telemetry/sf-collector/releases/download/$VERSION/libsysflow-$VERSION-x86_64.tgz
```

Install the libSysFlow package:

```bash
tar xzf libsysflow-musl-${SYSFLOW_VERSION}-x86_64.tar.gz && cp -r libsysflow-musl-${SYSFLOW_VERSION}-x86_64/usr/* /usr/.
```

## Compilation

After installation, you should have the following directory structure installed in your environment:

```bash
/usr/bin/docker-entrypoint.sh
/usr/bin/falco-driver-loader
/usr/include/falcosecurity
/usr/include/sysflow
/usr/lib/falcosecurity
/usr/lib/sysflow
/usr/src/dkms
/usr/src/falco-$FALCO_LIBS_VERSION
```

The `include` and `lib` directories contain the header files and static libraries that should be used to build a SysFlow consumer. `docker-entrypoint.sh` is provided for container-based deployments of the consumer, and the `falco-driver-loader` is the script used to load the kmod/bpf drivers. `dkms` sources are provided as a convenience and not required for compilation, and can be used to install dkms in case it's not available in the target environment. Similarly, we package the Falco driver sources in `/usr/src/falco-$FALCO_LIBS_VERSION` to enable local compilation of the drivers when these are not available or accessible in the remote driver repository.

The [Makefile](https://github.com/sysflow-telemetry/sf-collector/blob/dev/examples/Makefile) in the example application shows the `LDFLAGS` and `CFLAGS` needed to build a libSysFlow consumer, and provides an example of how to enable glibc and muslc (static) builds.

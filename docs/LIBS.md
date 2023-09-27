# LibSysFlow

LibSysFlow is a library for creating SysFlow consumers. It defines a concise API and export first-class SysFlow data types for consumers to transparently process SysFlow records and manage access to the underlying Falco libs and driver.

The main interface accepts a config object in which a callback function can be set to process SysFlow records. The config option sets optimal defaults that can be customized by the consumer. The library is packaged as a static (.a) library and distributed as an rpm/deb/tgz artifact with sf-collector releases (both glibc and musl flavors are available).

Additionally, libsysflow performs the checks to verify that the Falco driver is loaded and outputs an exception otherwise. Consumers load the Falco libs driver prior to running their main entrypoint, following the typical entrypoint recipe/script used by Falco and the SysFlow Collector.

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

## Public API

The public interface for the SysFlow libs offers two objects: `SysFlowConfig` and `SysFlowDriver`.

### SysFlowConfig

The `SysFlowConfig` object is a struct, which contains all settings for the libs and must be passed into the `SysFlowDriver` constructor.  A more detailed description of the configuration settings for `SysFlowConfig` can be found in [Advanced Usage](#advanced-usage).

| **Method** | **Description** |
|---|---|
| SysFlowConfig *sysflowlibscpp::InitializeSysFlowConfig() | Initializes the configuration object with a set of [default](#advanced-usage) values |

### SysFlowDriver

The `SysFlowDriver` object is the main object for collecting and exporting SysFlow data.  The driver also supports system call ingestion from the following sources: SCAP file, kernel module (live), and ebpf drivers (live). Configurations for file ingestion are currently set by the `SysFlowConfig` object. For live capture, the kernel module is loaded by default; however, one can use the ebpf driver by currently exporting the `FALCO_BPF_PROBE` environment variable (e.g., `export FALCO_BPF_PROBE=""`) before launching the binary. Alternatively, the CO-RE ebpf driver can be used by setting the `driverType` variable (which is set using the `-k` in the collector). Note that the kmod and ebpf drivers are launched by running the `falco-driver-loader` script described below; the CO-RE ebpf driver is embedded in libSysFlow and does not use the `falco-driver-loader` script. Finally, the driver offers a collection mode option, which determines which system calls are collected.  See the `collectionMode` attribute in the [Configuration](#configuration) section below for more details. The driver currently supports three export options: to avro encoded file, over unix domain socket, and call to user-defined callback function (see example above). Export options are configured using the `SysFlowConfig` option.

| **Method** | **Description** |
|---|---|
| SysFlowDriver(sysflowlibscpp::SysFlowConfig *config | Driver constructor configures the driver based on the settings in the [SysFlowConfig](#sysflowconfig) object.<br>Note: the constructor can throw a [SysFlowException](#exception-handling). |
| virtual ~SysFlowDriver() | Driver destructor |
| void exit() | Stops the driver data collection and export.  Typically called within a signal handler |
| int run() | Blocking function that runs the main collection loop. <br>Note: can throw a [SysFlowException](#exception-handling). Returns 0 on successful completion. |
| std::string getVersion() | returns a string representing the version number of the libraries |

## Installation

Binary packages (deb, rpm, tgz) for glibc- and musl-based build pipelines are available in the collector's [release assets](https://github.com/sysflow-telemetry/sf-collector/releases) (since release `$VERSION` >=0.5.0). This is going to install libSysFlow headers and the static libraries (.a) needed to link your application.

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

The [Makefile](https://github.com/sysflow-telemetry/sf-collector/blob/dev/examples/Makefile) in the example application shows the `LDFLAGS` and `CFLAGS` needed to build a libSysFlow consumer, and provides an example of how to enable glibc and musl (static) builds.

## Advanced Usage

### Configuration

The library configuration parameters are assigned defaults that should work well in most scenarios. They can be customized using the `SysFlowConfig` object.

| **Field** | **Type** | **Description** | **Default** |
|---|---|---|---|
| filterContainers | bool | Filter out all events related to containers | false |
| rotateInterval | int | Set rotation interval in secs which dictates how often a SysFlow header is emitted. Used for file rotations and also to clean caches to prevent leakages | 300 |
| exporterID | string | ID for the host | |
| nodeIP | string | IP for the host/node | |
| filePath | string | SysFlow output file path.  If path ends with a '/', this will be treated as a directory. If treated as directory, the name of the sysflow file will be a timestamp, and will be rotated every N seconds depending on the rotateInterval. If no '/' at end, and rotateInterval is set, path is treated as a file prefix, and  timestamp is concatenated. Set NULL if not using file output. | |
| socketPath | string | SysFlow unix socket file path.  Typically used in conjunction with the SysFlow processor to stream SysFlow over a socket. Set NULL if not using socket streaming | |
| scapInputPath | string | Scap input file path.  Used in offline mode to read from raw scap rather than tapping the kernel. Set NULL if using live kernel collection | |
| falcoFilter | string | String to set Falco-style filter on events being passed from the falco libs, to the SysFlow library | |
| samplingRatio | string | Sampling ratio used to determine which system calls to drop in the driver | 1 |
| criPath | string | CRI-O runtime socket path, needed for monitoring cri-o/containered container runtimes such as k8s and OCP | |
| criTO | int | CRI-O timeout.  Timeout in secs set when querying CRI-O socket for container metadata | 30 |
| enableStats | bool | Enable Process Flow collection.  Output Process Flow rather than individual thread clones | false |
| enableProcessFlow | bool | Only output File Flows and Events related to file objects.  Ignoring pipes, for example. | true |
| fileOnly | bool | Only output File Flows and Events related to file objects.  Ignoring pipes, for example. | true |
| fileReadMode | int | Set the file mode to determine which types of file related read flows are ignored to reduce event output. sets mode for reads: "0" enables recording all file reads as flows.  "1" disables all file reads. "2" disables recording file reads to noisy directories: "/proc/", "/dev/", "/sys/", "//sys/", "/lib/",  "/lib64/", "/usr/lib/", "/usr/lib64/" | 2 |
| dropMode | bool | Drop mode removes syscalls inside the kernel before they are passed up to the collector results in much better performance, less drops, but does remove mmaps from output. | true |
| callback | SysFlowCallback | Callback function, required for when using a custom callback function for SysFlow processing | |
| debugMode | bool | Debug mode turns on debug logging inside libsinsp  | false |
| k8sAPIURL | string | K8s API URL used to retrieve K8s state and K8s events (experimental) | |
| k8sAPICert | string | Path to K8s API Certificate (experimental) | |
| moduleChecks | bool | Run added module checks for better error checking | true |
| collectionMode | enum | Has three possible values: 1.) `SFFlowMode` for SysFlow mode which does full SysFlow collection as described in the spec. 2.) `SFConsumerMode` removes collection of `read`, `write` and `close` operations for FileFlows. 3.) `SFNoFiles` drops file flows and file events. The latter two options are a lighterweight collection mode for systems where CPU or drop issues may occur | `SFFlowMode` |
| appName | string | Sets the calling application name for logging purposes. | `sysflowlibs` |
| singleBufferDimension | int | This is the dimension that a single buffer in our drivers will have (BPF, kmod, modern BPF) Please note:  This number is expressed in bytes. This number must be a multiple of your system page size, otherwise the allocation will fail. If you leave `0`, every driver will set its internal default dimension. | 0 |
| cpuBuffers | int | Sets the number of CPU ring buffers to set up to collect system calls. Traditional eBPF automatically uses one per online CPU. This setting is only relevant for the CORE eBPF driver, and cannot be higher than the number of online CPUs available. Setting the value to `0` causes it to choose the number of online CPUs. | 0 |
| driverType | enum | Sets the driver type to `EBPF` (traditional ebpf driver), `KMOD` (kernel module), `CORE_EBPF` (CORE ebpf driver), `NO_DRIVER` (reading from a file). | `KMOD` |

### Exception Handling

The library exposes an exception class that contains error code that can be used by SysFlow consumers for logging and troubleshooting.

```cpp
SysFlowException(std::string message);
SysFlowException(std::string message, SysFlowError code)
      : std::runtime_error(message), m_code(code) {}
SysFlowError getErrorCode() { return m_code; }
```
The error codes are defined in an enum, as follows.

```cpp
enum SysFlowError {
  LibsError,
  ProbeAccessDenied,
  ProbeNotExist,
  ErrorReadingFileSystem,
  NameTooLong,
  ProbeCheckError,
  ProbeNotLoaded,
  DriverLibsMismatch,
  EventParsingError,
  ProcResourceNotFound,
  OperationNotSupported
};
```

### Logging

LibSysFlow uses [Glog](https://github.com/google/glog) for logging.

You can specify one of the following severity levels (in increasing order of severity): INFO, WARNING, ERROR.

You can also add verbose logging when you are chasing difficult bugs. LibSysFlow has two levels of verbose logging: 1 (DEBUG) and 2 (TRACE).

To control logging behavior, you can set flags via environment variables, prefixing the flag name with "GLOG_", e.g.

```bash
   GLOG_logtostderr=1 ./your_application
```

The following flags are most commonly used:

- logtostderr (bool, default=false): Log messages to stderr instead of logfiles.
- stderrthreshold (int, default=2, which is ERROR): Copy log messages at or above this level to stderr in addition to logfiles. The numbers of severity levels INFO, WARNING, ERROR, and FATAL are 0, 1, 2, and 3, respectively.
- minloglevel (int, default=0, which is INFO): Log messages at or above this level. Again, the numbers of severity levels INFO, WARNING, ERROR, and FATAL are 0, 1, 2, and 3, respectively.
- log_dir (string, default=""): If specified, logfiles are written into this directory instead of the default logging directory.
- v (int, default=0): Show all VLOG(m) messages for m less or equal the value of this flag. Overridable by --vmodule. See the section about verbose logging for more detail.
- vmodule (string, default=""): Per-module verbose level. The argument has to contain a comma-separated list of <module name>=<log level>. <module name> is a glob pattern (e.g., gfs* for all modules whose name starts with "gfs"), matched against the filename base (that is, name ignoring .cc/.h./-inl.h). <log level> overrides any value given by --v.


>---
> **Note** You can set binary flags to true by specifying 1, true, or yes (case insensitive). Also, you can set binary flags to false by specifying 0, false, or no (again, case insensitive).
>
>---


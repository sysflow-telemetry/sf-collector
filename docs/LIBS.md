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


# Example LibSysFlow consumer

This [example](callback.cpp) shows how to implement, build, and run a SysFlow consumer using LibSysFlow.

The application uses callback functions to process SysFlow records. E.g.,

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

## Pre-requisites

- docker (for building the application)
- llvm 12+ (if running the eBPF driver)

**Note:** SysFlow requires the kernel headers to be installed on the environment. You can check if kernel headers are installed by checking `/usr/src` in your closed environment. In debian systems, the package name is `linux-headers-generic` or `linux-headers-$(uname -r)`, and in rhel systems, the package name is `kernel-devel` or `kernel-devel-$(uname -r)`.

## Build

```bash
make build
```

## Running

To execute the application on a barehost, run:

```bash
run.sh
```

Otherwise, to launch the application in a container, run:

```bash
docker-run.sh
```



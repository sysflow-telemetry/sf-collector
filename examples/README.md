# Example: SysFlow consumer

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

### Falco drivers and driver loader requirements

Debian-based systems:

```bash
apt-get install -y apt-get install -y --no-install-recommends bash-completion bc bison clang ca-certificates curl dkms	flex gnupg2 gcc jq libc6-dev libelf-dev libmpx2 libssl-dev llvm	netcat xz-utils
```

RPM-based systems:

```bash
dnf install -y gcc gcc-c++ wget binutils bzip2 perl glibc-static diffutils kmod xz apr-devel apr-util-devel openssl-devel flex bison libstdc++-static bc llvm-toolset
```

**Note:** The Falco driver requires the kernel headers to be installed on the environment. You can check if kernel headers are installed by checking `/usr/src` in your closed environment. In debian systems, the package name is `linux-headers-generic` or `linux-headers-$(uname -r)`, and in rhel systems, the package name is `kernel-devel` or `kernel-devel-$(uname -r)`.

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

**Note:** The example uses the eBPF probe by default. To use the kmod probe, remove `-e FALCO_BPF_PROBE=""` from `docker-run.sh` and remove argument `bpf` passed to the falco-driver-loader in `run.sh`.



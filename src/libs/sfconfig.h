/** Copyright (C) 2022 IBM Corporation.
 *
 * Authors:
 * Frederico Araujo <frederico.araujo@ibm.com>
 * Teryl Taylor <terylt@ibm.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 **/

#ifndef _SF_CONFIG_LIBS_
#define _SF_CONFIG_LIBS_

#include "sysflow.h"
#include <stdint.h>

enum SFSysCallMode { SFFlowMode, SFConsumerMode, SFNoFilesMode };
enum DriverType { EBPF, CORE_EBPF, KMOD, NO_DRIVER };

using SysFlowCallback = std::function<void(
    sysflow::SFHeader *, sysflow::Container *, sysflow::Process *,
    sysflow::File *, sysflow::File *, sysflow::SysFlow *)>;

struct SysFlowConfig {
  // Filter out all events related to containers.
  bool filterContainers;
  // Set rotation interval in secs which dictates how often a SysFlow header is
  // emitted. Used for file rotations and also to clean caches to prevent
  // leakages. Typically set to 300 secs (5 mins)
  int rotateInterval;
  // ID for the host.
  std::string exporterID;
  // IP for the host/node.
  std::string nodeIP;
  // SysFlow output file path.  If path ends with a '/', this will be treated as
  // a directory. If treated as directory, the name of the sysflow file will be
  // a timestamp, and will be rotated. every N seconds depending on the
  // rotateInterval. If no '/' at end, and rotateInterval is set, path is
  // treated as a file prefix, and  timestamp is concatenated. Set null if not
  // using file output.
  std::string filePath;
  // SysFlow unix socket file path.  Typically used in conjunction with the
  // SysFlow processor to stream SysFlow over a socket. Set null if not use
  // socket streaming
  std::string socketPath;
  // Scap input file path.  Used in offline mode to read from raw scap rather
  // than tapping the kernel. Set null if using online kernel collection.
  std::string scapInputPath;
  // String to set Falco-style filter on events being passed from the falco
  // libs, to the SysFlow library
  std::string falcoFilter;
  // Sampling ratio used to determine which system calls to drop in the driver.
  int samplingRatio;
  // CRI-O runtime socket path, needed for monitoring cri-o/containered
  // container runtimes such as k8s and OCP.
  std::string criPath;
  // CRI-O timeout.  Timeout set when querying CRI-O socket for container
  // metadata.
  int criTO;
  // Enable printout of object collection stats at intervals.
  bool enableStats;
  // Enable Process Flow collection.  Output Process Flow rather than individual
  // thread clones.
  bool enableProcessFlow;
  // Only output File Flows and Events related to file objects.  Ignoring pipes,
  // for example.
  bool fileOnly;
  // Set the file mode to determine which types of file related read flows are
  // ignored to reduce event output. sets mode for reads: "0" enables recording
  // all file reads as flows.  "1" disables all file reads. "2" disables
  // recording file reads to noisy directories: "/proc/", "/dev/", "/sys/",
  // "//sys/",
  // "/lib/",  "/lib64/", "/usr/lib/", "/usr/lib64/"
  int fileReadMode;
  // Drop mode removes syscalls inside the kernel before they are passed up to
  // the collector results in much better performance, less drops, but does
  // remove mmaps from output.
  bool dropMode;
  // Callback function, required for when using a custom callback function for
  // SysFlow processing.
  SysFlowCallback callback;
  // Debug mode turns on debug logging inside libsinsp.
  bool debugMode;
  // K8s API URL used to retrieve K8s state and K8s events (experimental).
  std::string k8sAPIURL;
  // Path to K8s API Certificate (experimental).
  std::string k8sAPICert;
  // Run added module checks for better error checking.
  bool moduleChecks;
  // This is the dimension that a single buffer in our drivers will have. (BPF,
  // kmod, modern BPF) Please note:
  // - This number is expressed in bytes.
  // - This number must be a multiple of your system page size, otherwise the
  // allocation will fail.
  // - If you leave `0`, every driver will set its internal default dimension.
  uint64_t singleBufferDimension;
  // The app name used in glog logging.
  std::string appName;
  // Sets the current collection mode of the libs. There are currently two
  // modes: SFFlowMode: default mode supports full file and network flows.
  // SFConsumerMode: Lighterweight mode that doesn't collect
  // read/write/send/recv data (experimental, wip)
  SFSysCallMode collectionMode;
  // Sets the number of CPU ring buffers to set up to collect system calls.
  // Traditional eBPF automatically uses one per online CPU. This setting is
  // only relevant for the CORE eBPF, and cannot be higher than the number of
  // online CPUs available.  Setting the value to 0 causes it to choose the
  // number of online CPUs.
  ssize_t cpuBuffers;
  // Set the driver type to EBPF (traditional ebpf driver), KMOD (kernel
  // module), CORE_EBPF (CORE ebpf driver), NO_DRIVER (reading from a file)
  DriverType driverType;
}; // SysFlowConfig

#endif

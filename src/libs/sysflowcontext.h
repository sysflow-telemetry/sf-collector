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

#ifndef _SF_CONTEXT_
#define _SF_CONTEXT_
#include <string>

#include <ctime>

#include "logger.h"
#include "readonly.h"
#include "sfconfig.h"
#include "sysflow.h"
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <sinsp.h>
#include <unistd.h>

#define DRIVER_LOG "DRIVER_LOG"
#define NODE_IP "NODE_IP"
#define ENABLE_DROP_MODE "ENABLE_DROP_MODE"
#define FILE_READ_MODE "FILE_READ_MODE"
#define FILE_ONLY "FILE_ONLY"
#define ENABLE_STATS "ENABLE_STATS"
#define ENABLE_PROC_FLOW "ENABLE_PROC_FLOW"
#define SF_K8S_API_URL "SF_K8S_API_URL"
#define SF_K8S_API_CERT "SF_K8S_API_CERT"
#define SF_PROBE_BPF_FILEPATH ".falco/falco-bpf.o"
#define SF_BPF_ENV_VARIABLE "FALCO_BPF_PROBE"
#define DRIVER_HOME "HOME"

namespace context {

enum ProbeType { EBPF, KMOD, NO_PROBE };

class SysFlowContext {
private:
  // time_t m_start{};
  int m_nfExportInterval;
  int m_nfExpireInterval;
  bool m_offline;
  int m_statsInterval;
  string m_nodeIP;
  bool m_k8sEnabled;
  SysFlowCallback m_callback;
  SysFlowConfig *m_config;
  bool m_hasPrefix;
  ProbeType m_probeType;
  std::string m_ebpfProbe;
  sinsp *m_inspector;
  DEFINE_LOGGER();
  void detectProbeType();
  void checkModule();
  void openInspector();

public:  
  SysFlowContext(SysFlowConfig *config);
  virtual ~SysFlowContext();
  uint64_t timeStamp{};
  string getExporterID();
  string getNodeIP();
  SysFlowCallback getCallback() { return m_callback; }
  inline void setNodeIP(string nodeIP) { m_nodeIP = nodeIP; }  
  inline bool isOffline() { return m_offline; }
  inline bool hasCallback() { return m_callback != nullptr; }
  inline sinsp *getInspector() { return m_inspector; }
  inline int getNFExportInterval() { return m_nfExportInterval; }
  inline int getNFExpireInterval() { return m_nfExpireInterval; }
  inline string getOutputFile() { return m_config->filePath; }
  inline string getSocketFile() { return m_config->socketPath; }
  inline bool isDomainSocket() { return !m_config->socketPath.empty(); }
  inline bool isOutputFile() { return !m_config->filePath.empty(); }
  inline string getScapFile() { return m_config->scapInputPath; }
  inline bool hasPrefix() { return m_hasPrefix; }
  inline int getFileDuration() { return m_config->rotateInterval; }
  inline bool isFilterContainers() { return m_config->filterContainers; }
  inline bool isStatsEnabled() { return m_config->enableStats; }
  /*inline void enableStats() { m_stats = true; }*/
  inline bool isProcessFlowEnabled() { return m_config->enableProcessFlow; }
  inline int getStatsInterval() { return m_statsInterval; }
  inline bool isFileOnly() { return m_config->fileOnly; }
  inline int getFileRead() { return m_config->fileReadMode; }
  inline bool isK8sEnabled() { return m_k8sEnabled; }
};
} // namespace context

#endif

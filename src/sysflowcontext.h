/** Copyright (C) 2019 IBM Corporation.
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
#define ENABLE_PROC_FLOW "ENABLE_PROC_FLOW"
#define SF_K8S_API_URL "SF_K8S_API_URL"
#define SF_K8S_API_CERT "SF_K8S_API_CERT"

namespace context {
class SysFlowContext {
private:
  bool m_filterCont;
  // time_t m_start{};
  int m_fileDuration;
  bool m_hasPrefix;
  string m_outputFile;
  string m_socketFile;
  string m_scapFile;
  uint32_t m_samplingRatio;
  string m_exporterID;
  sinsp *m_inspector;
  int m_nfExportInterval;
  int m_nfExpireInterval;
  bool m_offline;
  string m_filter;
  string m_criPath;
  int m_criTO;
  bool m_stats;
  int m_statsInterval;
  bool m_processFlow;
  bool m_fileOnly;
  int m_fileRead;
  string m_nodeIP;
  bool m_k8sEnabled;
  DEFINE_LOGGER();

public:
  SysFlowContext(bool fCont, int fDur, string oFile, string socketFile,
                 const string &sFile, uint32_t samplingRatio, string exporterID,
                 string filter, string criPath, int criTO);
  virtual ~SysFlowContext();
  uint64_t timeStamp{};
  string getExporterID();
  string getNodeIP();
  inline bool isOffline() { return m_offline; }
  inline sinsp *getInspector() { return m_inspector; }
  inline int getNFExportInterval() { return m_nfExportInterval; }
  inline int getNFExpireInterval() { return m_nfExpireInterval; }
  inline string getOutputFile() { return m_outputFile; }
  inline string getSocketFile() { return m_socketFile; }
  inline bool isDomainSocket() { return !m_socketFile.empty(); }
  inline bool isOutputFile() { return !m_outputFile.empty(); }
  inline string getScapFile() { return m_scapFile; }
  inline bool hasPrefix() { return m_hasPrefix; }
  inline int getFileDuration() { return m_fileDuration; }
  inline bool isFilterContainers() { return m_filterCont; }
  inline bool isStatsEnabled() { return m_stats; }
  inline void enableStats() { m_stats = true; }
  inline bool isProcessFlowEnabled() { return m_processFlow; }
  inline int getStatsInterval() { return m_statsInterval; }
  inline bool isFileOnly() { return m_fileOnly; }
  inline int getFileRead() { return m_fileRead; }
  inline bool isK8sEnabled() { return m_k8sEnabled; }
};
} // namespace context

#endif

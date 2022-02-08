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

#include "sysflowcontext.h"
#include <utility>

using context::SysFlowContext;

CREATE_LOGGER(SysFlowContext, "sysflow.sysflowcontext");

SysFlowContext::SysFlowContext(bool fCont, int fDur, string oFile,
                               string socketFile, const string &sFile,
                               uint32_t samplingRatio, string expID,
                               string filter, string criPath, int criTO)
    : m_filterCont(fCont), m_fileDuration(fDur), m_outputFile(oFile),
      m_socketFile(socketFile), m_scapFile(sFile),
      m_samplingRatio(samplingRatio), m_exporterID(std::move(expID)),
      m_nfExportInterval(30), m_nfExpireInterval(60), m_offline(false),
      m_filter(std::move(filter)), m_criPath(std::move(criPath)),
      m_criTO(criTO), m_stats(false), m_statsInterval(30), m_processFlow(false),
      m_fileOnly(false), m_fileRead(0), m_nodeIP() {
  m_inspector = new sinsp();
  m_inspector->set_hostname_and_port_resolution_mode(false);
  if (!m_filter.empty()) {
    m_inspector->set_filter(m_filter);
  }
  if (!m_criPath.empty()) {
    m_inspector->set_cri_socket_path(m_criPath);
  }
  if (m_criTO > 0) {
    m_inspector->set_cri_timeout(m_criTO);
  }
  const char *envP = std::getenv(DRIVER_LOG);
  if (envP != nullptr && strcmp(envP, "1") == 0) {
    m_inspector->set_log_stderr();
    m_inspector->set_min_log_severity(sinsp_logger::severity::SEV_DEBUG);
  }
  const char *ip = std::getenv(NODE_IP);
  if (ip != nullptr && std::strlen(ip) > 0) {
    m_nodeIP = std::string(ip);
  }
  m_inspector->open(m_scapFile);
  const char *drop = std::getenv(ENABLE_DROP_MODE);
  if (m_scapFile.empty() && drop != nullptr && std::strlen(drop) > 0) {
    std::cout << "Starting dropping mode with sampling rate: " << samplingRatio
              << std::endl;
    m_inspector->start_dropping_mode(m_samplingRatio);
  }
  const char *fileOnly = std::getenv(FILE_ONLY);
  if (fileOnly != nullptr && strcmp(fileOnly, "1") == 0) {
    std::cout << "Enabled file only mode!" << std::endl;
    m_fileOnly = true;
  }

  const char *procflow = std::getenv(ENABLE_PROC_FLOW);
  if (procflow != nullptr && strcmp(procflow, "1") == 0) {
    std::cout << "Enabled process flow mode!" << std::endl;
    m_processFlow = true;
  }

  const char *fileRead = std::getenv(FILE_READ_MODE);
  if (fileRead == nullptr || strcmp(fileRead, "0") == 0) {
    std::cout << "Enabled all file reads!" << std::endl;
    m_fileRead = FILE_READS_ENABLED;
  } else if (fileRead != nullptr && strcmp(fileRead, "1") == 0) {
    std::cout << "Disabled all file reads!" << std::endl;
    m_fileRead = FILE_READS_DISABLED;
  } else if (fileRead != nullptr && strcmp(fileRead, "2") == 0) {
    std::cout << "Disabled file reads to dirs: /proc/, /usr/lib/, /usr/lib64/, "
                 "/lib64/, /lib/, /dev/, /sys/, //sys/"
              << std::endl;
    m_fileRead = FILE_READS_SELECT;
  } else {
    SF_WARN(
        m_logger,
        "FILE_READ_MODE must be set to 0 = enable all file reads, 1 = disable "
        "all file reads, or 2 = disable file reads to certain directories")
  }
  if (m_scapFile.empty()) {
    m_inspector->set_snaplen(0);
  }
  m_offline = !sFile.empty();
  m_hasPrefix = (oFile.back() != '/');
}

SysFlowContext::~SysFlowContext() {
  if (m_inspector != nullptr) {
    m_inspector->close();
    delete m_inspector;
  }
}
string SysFlowContext::getExporterID() {
  if (m_exporterID.empty()) {
    const scap_machine_info *mi = m_inspector->get_machine_info();
    if (mi != nullptr) {
      m_exporterID = mi->hostname;
    } else {
      char host[257];
      memset(host, 0, 257);
      if (gethostname(host, 256)) {
        SF_ERROR(m_logger,
                 "Error calling gethostname for sysflow header. Error Code: "
                     << std::strerror(errno));
        exit(1);
      }
      m_exporterID = host;
    }
  }
  return m_exporterID;
}

string SysFlowContext::getNodeIP() { return m_nodeIP; }

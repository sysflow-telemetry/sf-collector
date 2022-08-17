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

SysFlowContext::SysFlowContext(SysFlowConfig *config)
    : m_nfExportInterval(30), m_nfExpireInterval(60), m_offline(false),
      m_statsInterval(30), m_nodeIP(), m_k8sEnabled(false) {
  m_inspector = new sinsp();

  m_inspector->set_buffer_format(sinsp_evt::PF_NORMAL);

  m_inspector->set_hostname_and_port_resolution_mode(false);
  if (!config->falcoFilter.empty()) {
    m_inspector->set_filter(config->falcoFilter);
  }
  if (!config->criPath.empty()) {
    m_inspector->set_cri_socket_path(config->criPath);
  }
  if (config->criTO > 0) {
    m_inspector->set_cri_timeout(config->criTO);
  }
  const char *envP = std::getenv(DRIVER_LOG);
  if ((envP != nullptr && strcmp(envP, "1") == 0) ||
      (envP == nullptr && config->debugMode)) {
    m_inspector->set_log_stderr();
    m_inspector->set_min_log_severity(sinsp_logger::severity::SEV_DEBUG);
  }
  const char *ip = std::getenv(NODE_IP);
  if (ip != nullptr && std::strlen(ip) > 0) {
    m_nodeIP = std::string(ip);
  } else if (ip == nullptr && !config->nodeIP.empty()) {
    m_nodeIP = config->nodeIP;
  }
  m_inspector->open(config->scapInputPath);
  const char *drop = std::getenv(ENABLE_DROP_MODE);
  if (config->scapInputPath.empty() &&
      ((drop != nullptr && strcmp(drop, "1") == 0) ||
       (drop == nullptr && config->dropMode))) {
    std::cout << "Starting dropping mode with sampling rate: "
              << config->samplingRatio << std::endl;
    m_inspector->start_dropping_mode(config->samplingRatio);
  }
  const char *fileOnly = std::getenv(FILE_ONLY);
  if (fileOnly != nullptr && strcmp(fileOnly, "1") == 0) {
    config->fileOnly = true;
  } else if (fileOnly != nullptr) {
    config->fileOnly = false;
  }
  if (config->fileOnly) {
    std::cout << "Enabled file only mode!" << std::endl;
  }
  const char *procflow = std::getenv(ENABLE_PROC_FLOW);
  if (procflow != nullptr && strcmp(procflow, "1") == 0) {
    config->enableProcessFlow = true;
  } else if (procflow != nullptr) {
    config->enableProcessFlow = false;
  }
  if (config->enableProcessFlow) {
    std::cout << "Enabled process flow mode!" << std::endl;
  }

  const char *fileRead = std::getenv(FILE_READ_MODE);
  if (fileRead != nullptr && strcmp(fileRead, "0") == 0) {
    std::cout << "Enabled all file reads!" << std::endl;
    config->fileReadMode = FILE_READS_ENABLED;
  } else if (fileRead != nullptr && strcmp(fileRead, "1") == 0) {
    std::cout << "Disabled all file reads!" << std::endl;
    config->fileReadMode = FILE_READS_DISABLED;
  } else if (fileRead != nullptr && strcmp(fileRead, "2") == 0) {
    std::cout << "Disabled file reads to dirs: /proc/, /usr/lib/, /usr/lib64/, "
                 "/lib64/, /lib/, /dev/, /sys/, //sys/"
              << std::endl;
    config->fileReadMode = FILE_READS_SELECT;
  } else {
    std::cout
        << "File Read Mode was set to: " << config->fileReadMode
        << " Modes are:  0 = enable all file reads, 1 = disable "
        << "all file reads, or 2 = disable file reads to certain directories "
        << std::endl;
  }

  char *k8sAPIURL = getenv(SF_K8S_API_URL);
  char *k8sAPICert = getenv(SF_K8S_API_CERT);
  string *k8sURL = nullptr;
  string *k8sCert = nullptr;
  if (k8sAPIURL != nullptr) {
    k8sURL = new string(k8sAPIURL);
    if (k8sAPICert != nullptr) {
      k8sCert = new string(k8sAPICert);
    }
  } else if (!config->k8sAPIURL.empty()) {
    k8sURL = &(config->k8sAPIURL);
    k8sCert = &(config->k8sAPICert);
  }

  if (k8sURL != nullptr) {
    std::cout << "Initing k8s client. URL: " << k8sURL
              << " and certificate: " << k8sCert << std::endl;
    m_inspector->init_k8s_client(k8sURL, k8sCert, &config->exporterID, true);
    m_inspector->set_internal_events_mode(true);
    m_k8sEnabled = true;
  }

  if (config->scapInputPath.empty()) {
    m_inspector->set_snaplen(0);
  }
  m_offline = !config->scapInputPath.empty();
  m_hasPrefix = (config->filePath.back() != '/');
  m_callback = config->callback;
  m_config = config;
}

SysFlowContext::~SysFlowContext() {
  if (m_inspector != nullptr) {
    m_inspector->close();
    delete m_inspector;
  }
}
string SysFlowContext::getExporterID() {
  if (m_config->exporterID.empty()) {
    const scap_machine_info *mi = m_inspector->get_machine_info();
    if (mi != nullptr) {
      m_config->exporterID = mi->hostname;
    } else {
      char host[257];
      memset(host, 0, 257);
      if (gethostname(host, 256)) {
        SF_ERROR(m_logger,
                 "Error calling gethostname for sysflow header. Error Code: "
                     << std::strerror(errno));
        exit(1);
      }
      m_config->exporterID = host;
    }
  }
  return m_config->exporterID;
}

string SysFlowContext::getNodeIP() { return m_nodeIP; }

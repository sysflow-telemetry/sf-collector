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

#include "sysflowcontext.h"
#include "engine/bpf/bpf_public.h"
#include "modutils.h"
#include <utility>

using context::SysFlowContext;

CREATE_LOGGER(SysFlowContext, "sysflow.sysflowcontext");

SysFlowContext::SysFlowContext(SysFlowConfig *config)
    : m_nfExportInterval(30), m_nfExpireInterval(60), m_offline(false),
      m_statsInterval(30), m_nodeIP(), m_k8sEnabled(false),
      m_probeType(NO_PROBE) {

  m_config = config;
  m_offline = !config->scapInputPath.empty();
  if (!m_offline) {
    detectProbeType();
    checkModule();
  }

  m_config = config;
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

  const char *statsEnv = std::getenv(ENABLE_STATS);
  if (statsEnv != nullptr && strcmp(statsEnv, "1") == 0) {
    config->enableStats = true;
  }

  const char *ip = std::getenv(NODE_IP);
  if (ip != nullptr && std::strlen(ip) > 0) {
    m_nodeIP = std::string(ip);
  } else if (ip == nullptr && !config->nodeIP.empty()) {
    m_nodeIP = config->nodeIP;
  }

  openInspector();

  const char *drop = std::getenv(ENABLE_DROP_MODE);
  if (config->scapInputPath.empty() &&
      ((drop != nullptr && strcmp(drop, "1") == 0) ||
       (drop == nullptr && config->dropMode))) {
    SF_INFO(m_logger, "Starting dropping mode with sampling rate: "
                          << config->samplingRatio)
    m_inspector->start_dropping_mode(config->samplingRatio);
  }

  const char *fileOnly = std::getenv(FILE_ONLY);
  if (fileOnly != nullptr && strcmp(fileOnly, "1") == 0) {
    config->fileOnly = true;
  } else if (fileOnly != nullptr) {
    config->fileOnly = false;
  }

  if (config->fileOnly) {
    SF_INFO(m_logger, "Enabled file only mode")
  }

  const char *procflow = std::getenv(ENABLE_PROC_FLOW);
  if (procflow != nullptr && strcmp(procflow, "1") == 0) {
    config->enableProcessFlow = true;
  } else if (procflow != nullptr) {
    config->enableProcessFlow = false;
  }

  if (config->enableProcessFlow) {
    SF_INFO(m_logger, "Enabled process flow mode")
  }

  const char *fileRead = std::getenv(FILE_READ_MODE);
  if (fileRead != nullptr && strcmp(fileRead, "0") == 0) {
    SF_INFO(m_logger, "Enabled all file reads")
    config->fileReadMode = FILE_READS_ENABLED;
  } else if (fileRead != nullptr && strcmp(fileRead, "1") == 0) {
    SF_INFO(m_logger, "Disabled all file reads")
    config->fileReadMode = FILE_READS_DISABLED;
  } else if (fileRead != nullptr && strcmp(fileRead, "2") == 0) {
    SF_INFO(m_logger,
            "Disabled file reads to dirs: /proc/, /usr/lib/, /usr/lib64/, "
            "/lib64/, /lib/, /dev/, /sys/, //sys/")
    config->fileReadMode = FILE_READS_SELECT;
  } else {
    SF_INFO(m_logger,
            "File Read Mode was set to: "
                << config->fileReadMode
                << " Modes are:  0 = enable all file reads, 1 = disable "
                << "all file reads, or 2 = disable file reads to certain "
                   "directories")
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
    SF_INFO(m_logger, "Initing k8s client. URL: "
                          << k8sURL << " and certificate: " << k8sCert)
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

void SysFlowContext::checkModule() {
  if (!m_config->moduleChecks) {
    return;
  }
  switch (m_probeType) {
  case KMOD: {
    modutils::checkForFalcoKernMod();
    break;
  }
  case EBPF: {
    modutils::checkProbeExistsPermits(m_ebpfProbe);
    break;
  }
  default: {
    SF_WARN(m_logger, "Probe type currently "
                          << m_probeType
                          << " not handled by check module operation")
    break;
  }
  }
}

void SysFlowContext::openInspector() {
  switch (m_probeType) {
  case KMOD:
    m_inspector->open_kmod(m_config->singleBufferDimension);
    break;
  case EBPF:
    m_inspector->open_bpf(m_config->singleBufferDimension, m_ebpfProbe.c_str());
    break;
  case NO_PROBE:
    m_inspector->open_savefile(m_config->scapInputPath.c_str(), 0);
  default:
    SF_WARN(m_logger, "Unsupported driver " << m_probeType)
    break;
  }
}

void SysFlowContext::detectProbeType() {
  const char *probe = std::getenv(SF_BPF_ENV_VARIABLE);
  if (probe == nullptr) {
    m_probeType = KMOD;
  } else {
    m_probeType = EBPF;
    if (strlen(probe) != 0) {
      m_ebpfProbe = std::string(probe);
    } else {
      const char *home = getenv(DRIVER_HOME);
      if (!home) {
        return;
      }
      m_ebpfProbe = std::string(home) + std::string("/") +
                    std::string(SF_PROBE_BPF_FILEPATH);
    }
  }
}

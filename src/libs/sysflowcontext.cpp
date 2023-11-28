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
#include "sfmodes.h"
#include <utility>

using context::SysFlowContext;

CREATE_LOGGER(SysFlowContext, "sysflow.sysflowcontext");

SysFlowContext::SysFlowContext(SysFlowConfig *config)
    : m_nfExportInterval(30), m_nfExpireInterval(60), m_offline(false),
      m_statsInterval(30), m_nodeIP(), m_k8sEnabled(false) {
  m_config = config;
  m_offline = !config->scapInputPath.empty();
  if (!m_offline) {
    loadDriverInfo();
    checkModule();
  }

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

  auto ppm_sc = getSyscallSet();
  auto syscalls = libsinsp::events::sc_set_to_sc_names(ppm_sc);

  SF_DEBUG(m_logger, "List of Syscalls after enforcement:")
  for (auto it : syscalls) {
    SF_DEBUG(m_logger, it);
  }

  openInspector(ppm_sc);

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

  if (config->fileOnly && !isNoFilesMode()) {
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

  if (!isNoFilesMode()) {
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
  }

  char *k8sAPIURL = getenv(SF_K8S_API_URL);
  char *k8sAPICert = getenv(SF_K8S_API_CERT);
  std::string *k8sURL = nullptr;
  std::string *k8sCert = nullptr;
  if (k8sAPIURL != nullptr) {
    k8sURL = new std::string(k8sAPIURL);
    if (k8sAPICert != nullptr) {
      k8sCert = new std::string(k8sAPICert);
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
  if (m_config != nullptr) {
    delete m_config;
    m_config = nullptr;
  }
}

std::string SysFlowContext::getExporterID() {
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

std::string SysFlowContext::getNodeIP() { return m_nodeIP; }

void SysFlowContext::checkModule() {
  if (!m_config->moduleChecks) {
    return;
  }
  switch (m_config->driverType) {
  case KMOD: {
    modutils::checkForFalcoKernMod();
    break;
  }
  case EBPF: {
    modutils::checkProbeExistsPermits(m_ebpfProbe);
    break;
  }
  case CORE_EBPF: {
    // TODO verify the CORE driver exists
    break;
  }
  default: {
    SF_WARN(m_logger, "Driver type currently "
                          << m_config->driverType
                          << " not handled by check module operation")
    break;
  }
  }
}

void SysFlowContext::openInspector(libsinsp::events::set<ppm_sc_code> ppm_sc) {
  std::string collectionMode;
  if (m_config->collectionMode == SFFlowMode) {
    collectionMode = "flow mode";
  } else if (m_config->collectionMode == SFConsumerMode) {
    collectionMode = "consumer mode";
  } else {
    collectionMode = "no files mode";
  }
  ssize_t onlineCPUs = 0;
  switch (m_config->driverType) {
  case KMOD:
    SF_INFO(m_logger, "Opening kmod driver in "
                          << collectionMode << " monitoring " << ppm_sc.size()
                          << " system calls.")
    m_inspector->open_kmod(m_config->singleBufferDimension, ppm_sc);
    break;
  case EBPF:
    SF_INFO(m_logger, "Opening ebpf driver in "
                          << collectionMode << " monitoring " << ppm_sc.size()
                          << " system calls.")
    m_inspector->open_bpf(m_ebpfProbe, m_config->singleBufferDimension, ppm_sc);
    break;
  case CORE_EBPF:
    SF_INFO(m_logger, "Opening CORE ebpf driver in "
                          << collectionMode << " monitoring " << ppm_sc.size()
                          << " system calls.")
    onlineCPUs = sysconf(_SC_NPROCESSORS_ONLN);
    if (m_config->cpuBuffers > onlineCPUs || m_config->cpuBuffers == 0) {
      if (m_config->cpuBuffers > onlineCPUs) {
        SF_INFO(
            m_logger,
            "Configured number CPU buffers exceeds maximum available online: "
                << m_config->cpuBuffers)
      }
      SF_INFO(m_logger, "Configuring number of CPU buffers to max available: "
                            << onlineCPUs)
      m_config->cpuBuffers = onlineCPUs;
    } else {
      SF_INFO(m_logger,
              "Configuring number of CPU buffers to: " << m_config->cpuBuffers)
    }
    m_inspector->open_modern_bpf(m_config->singleBufferDimension,
                                 m_config->cpuBuffers, true, ppm_sc);
    break;
  case NO_DRIVER:
    m_inspector->open_savefile(m_config->scapInputPath, 0);
    break;
  default:
    SF_WARN(m_logger, "Unsupported driver " << m_config->driverType)
    break;
  }
}

void SysFlowContext::loadDriverInfo() {
  if (m_config->driverType == KMOD || m_config->driverType == CORE_EBPF) {
    return;
  }
  const char *driver = std::getenv(SF_BPF_ENV_VARIABLE);
  if (driver != nullptr) {
    m_config->driverType = EBPF;
    if (strlen(driver) != 0) {
      m_ebpfProbe = std::string(driver);
    } else {
      const char *home = getenv(DRIVER_HOME);
      if (!home) {
        return;
      }
      m_ebpfProbe = std::string(home) + std::string("/") +
                    std::string(SF_PROBE_BPF_FILEPATH);
    }
  } else {
    m_config->driverType = KMOD;
    SF_WARN(m_logger, "Driver set to kernel module. "
                          << "Environment variable '" << SF_BPF_ENV_VARIABLE
                          << "' must be set to enable the eBPF driver.")
  }
}

libsinsp::events::set<ppm_sc_code>
SysFlowContext::getSyscallSet(libsinsp::events::set<ppm_sc_code> ppmScSet) {
  auto scMode = SF_FLOW_SC_SET;
  if (m_config->collectionMode == SFSysCallMode::SFConsumerMode) {
    SF_INFO(m_logger, "SysFlow configured for consumer mode.")
    scMode = SF_CONSUMER_SC_SET;
  } else if (m_config->collectionMode == SFSysCallMode::SFNoFilesMode) {
    SF_INFO(m_logger, "SysFlow configured for no files mode.")
    scMode = SF_NO_FILES_SC_SET;
  }

  auto scModeSet = libsinsp::events::set<ppm_sc_code>::from(scMode);
  auto syscalls = libsinsp::events::sc_set_to_sc_names(scModeSet);
  SF_DEBUG(m_logger, "Syscall List before enforcement:")
  for (auto it : syscalls) {
    SF_DEBUG(m_logger, it)
  }

  static auto sinspStatePpmSc = libsinsp::events::sinsp_state_sc_set();
  static auto finalSet = scModeSet.merge(sinspStatePpmSc);
  return ppmScSet.merge(finalSet);
}

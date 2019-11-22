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
                               const string &sFile, string schFile,
                               string expID, string filter,
                               string criPath, int criTO)
    : m_filterCont(fCont), m_fileDuration(fDur), m_outputFile(oFile),
      m_scapFile(sFile), m_schemaFile(std::move(schFile)),
      m_exporterID(std::move(expID)), m_nfExportInterval(30),
      m_nfExpireInterval(30), m_filter(std::move(filter)), 
      m_criPath(std::move(criPath)), m_criTO(criTO), m_stats(false), 
      m_statsInterval(30), m_domainSock(false) {
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
  m_inspector->open(m_scapFile);
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

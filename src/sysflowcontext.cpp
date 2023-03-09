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
#ifdef SYSDIG
#include "sysdig/sdinspector.h"
#endif
#ifdef CDM
#include "cdm/cdminspector.h"
#endif 
#include "containercontext.h"
#include "filecontext.h"
#include "processcontext.h"
#include <utility>

using context::SysFlowContext;

CREATE_LOGGER(SysFlowContext, "sysflow.sysflowcontext");

SysFlowContext::SysFlowContext(bool fCont, int fDur, string oFile,
                               const string &sFile, string schFile,
                               string expID, string filter, string criPath,
                               int criTO)
    : m_filterCont(fCont), m_fileDuration(fDur), m_outputFile(std::move(oFile)),
      m_inputFile(std::move(sFile)), m_schemaFile(std::move(schFile)),
      m_exporterID(std::move(expID)), m_nfExportInterval(30),
      m_nfExpireInterval(60), m_offline(false), m_filter(std::move(filter)),
      m_criPath(std::move(criPath)), m_criTO(criTO), m_stats(false),
      m_statsInterval(30), m_domainSock(false), m_processFlow(false) {
  m_offline = !m_inputFile.empty();
  m_hasPrefix = (m_outputFile.back() != '/');
}

SysFlowContext::~SysFlowContext() {
  m_inspector->cleanup();
  if (m_inspector != nullptr) {
    delete m_inspector;
  }
}
void SysFlowContext::init(process::ProcessContext *procCxt,
                          file::FileContext *fileCxt,
                          container::ContainerContext *contCxt) {
#ifdef SYSDIG
  m_inspector = new sysdig::SDInspector(this, procCxt, fileCxt, contCxt);
#endif
#ifdef CDM 
  m_inspector = new cdm::CDMInspector(this, procCxt, fileCxt, contCxt);
#endif
  m_inspector->init();
}

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

#include "dataflowprocessor.h"

using namespace dataflow;

CREATE_LOGGER(DataFlowProcessor, "sysflow.dataflow");

DataFlowProcessor::DataFlowProcessor(SysFlowContext *cxt, SysFlowWriter *writer,
                                     process::ProcessContext *processCxt,
                                     file::FileContext *fileCxt)
    : m_dfSet() {
  m_cxt = cxt;
  m_netflowPrcr =
      new networkflow::NetworkFlowProcessor(cxt, writer, processCxt, &m_dfSet);
  m_fileflowPrcr = new fileflow::FileFlowProcessor(cxt, writer, processCxt,
                                                   &m_dfSet, fileCxt);
  m_fileevtPrcr =
      new fileevent::FileEventProcessor(writer, processCxt, fileCxt);
  m_lastCheck = 0;
}

DataFlowProcessor::~DataFlowProcessor() {
  if (m_netflowPrcr != nullptr) {
    delete m_netflowPrcr;
  }
  if (m_fileflowPrcr != nullptr) {
    delete m_fileflowPrcr;
  }
  if (m_fileevtPrcr != nullptr) {
    delete m_fileevtPrcr;
  }
}

int DataFlowProcessor::handleDataEvent(sinsp_evt *ev, OpFlags flag) {
  sinsp_fdinfo_t *fdinfo = ev->get_fd_info();

  if (fdinfo == nullptr) {
    SF_DEBUG(
        m_logger,
        "Event: " << ev->get_name()
                  << " doesn't have an fdinfo associated with it! ErrorCode: "
                  << utils::getSyscallResult(ev));
    if (IS_FILE_EVT(flag)) {
      return m_fileevtPrcr->handleFileFlowEvent(ev, flag);
    } else if (flag == OP_MMAP) {
      return m_fileflowPrcr->handleFileFlowEvent(ev, flag);
    }
    if (fdinfo == nullptr) {
      SF_DEBUG(m_logger, " Returning 1");
      return 1;
    }
  }
  if (fdinfo->is_ipv4_socket() || fdinfo->is_ipv6_socket()) {
    return m_netflowPrcr->handleNetFlowEvent(ev, flag);
  } else if (IS_FILE_EVT(flag)) {
    return m_fileevtPrcr->handleFileFlowEvent(ev, flag);
  } else {
    return m_fileflowPrcr->handleFileFlowEvent(ev, flag);
  }
  return 2;
}

int DataFlowProcessor::removeAndWriteDFFromProc(ProcessObj *proc, int64_t tid) {
  int total = m_fileflowPrcr->removeAndWriteFFFromProc(proc, tid);
  return (total + m_netflowPrcr->removeAndWriteNFFromProc(proc, tid));
}

int DataFlowProcessor::checkForExpiredRecords() {
  time_t now = utils::getCurrentTime(m_cxt);
  if (m_lastCheck == 0) {
    m_lastCheck = now;
    return 0;
  }
  if (difftime(now, m_lastCheck) < 1.0) {
    return 0;
  }
  m_lastCheck = now;
  int i = 0;
  SF_DEBUG(m_logger, "Checking expired Flows!!!....");
  for (auto it = m_dfSet.begin(); it != m_dfSet.end();) {
    SF_DEBUG(m_logger, "Checking flow with exportTime: " << (*it)->exportTime
                                                         << " Now: " << now);
    if ((*it)->exportTime <= now) {
      SF_DEBUG(m_logger, "Exporting flow!!! ");
      if (difftime(now, (*it)->lastUpdate) >= m_cxt->getNFExpireInterval()) {
        if ((*it)->isNetworkFlow) {
          m_netflowPrcr->removeNetworkFlow((*it));
        } else {
          m_fileflowPrcr->removeFileFlow((*it));
        }
        it = m_dfSet.erase(it);
      } else {
        if ((*it)->isNetworkFlow) {
          m_netflowPrcr->exportNetworkFlow((*it), now);
        } else {
          m_fileflowPrcr->exportFileFlow((*it), now);
        }
        DataFlowObj *dfo = (*it);
        it = m_dfSet.erase(it);
        dfo->exportTime = utils::getExportTime(m_cxt);
        m_dfSet.insert(dfo);
      }
      i++;
    } else {
      break;
    }
  }
  return i;
}

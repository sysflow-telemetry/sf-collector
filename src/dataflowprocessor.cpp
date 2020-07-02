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

using dataflow::DataFlowProcessor;

CREATE_LOGGER(DataFlowProcessor, "sysflow.dataflow");

DataFlowProcessor::DataFlowProcessor(context::SysFlowContext *cxt,
                                     writer::SysFlowWriter *writer,
                                     process::ProcessContext *processCxt,
                                     file::FileContext *fileCxt)
    : m_dfSet() {
  m_cxt = cxt;
  m_procCxt = processCxt;
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

int DataFlowProcessor::handleDataEvent(api::SysFlowEvent *ev) {
  api::SysFlowFileDescInfo *fdinfo = ev->getFileDescInfo();

  if (fdinfo == nullptr) {
    SF_DEBUG(
        m_logger,
        "Event: " << ev->getName()
                  << " doesn't have an fdinfo associated with it! ErrorCode: "
                  << ev->getSysCallResult());
    if (IS_FILE_EVT(ev->opFlag)) {
      return m_fileevtPrcr->handleFileFlowEvent(ev);
    } else if (ev->opFlag == OP_MMAP) {
      return m_fileflowPrcr->handleFileFlowEvent(ev);
    }
    if (fdinfo == nullptr) {
      SF_DEBUG(m_logger, " Returning 1");
      return 1;
    }
  }
  if (fdinfo->isIPSocket()) {
    return m_netflowPrcr->handleNetFlowEvent(ev);
  } else if (IS_FILE_EVT(ev->opFlag)) {
    return m_fileevtPrcr->handleFileFlowEvent(ev);
  } else {
    return m_fileflowPrcr->handleFileFlowEvent(ev);
  }
  return 2;
}

int DataFlowProcessor::removeAndWriteDFFromProc(ProcessObj *proc, int64_t tid) {
  int total = m_fileflowPrcr->removeAndWriteFFFromProc(proc, tid);
  return (total + m_netflowPrcr->removeAndWriteNFFromProc(proc, tid));
}

void DataFlowProcessor::printFlowStats() {
  m_procCxt->printStats();
  SF_INFO(m_logger, "DF Set: " << m_dfSet.size());
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

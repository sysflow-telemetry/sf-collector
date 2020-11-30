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

#include "controlflowprocessor.h"

using controlflow::ControlFlowProcessor;

CREATE_LOGGER(ControlFlowProcessor, "sysflow.procflow");

ControlFlowProcessor::ControlFlowProcessor(
    context::SysFlowContext *cxt, writer::SysFlowWriter *writer,
    process::ProcessContext *processCxt, dataflow::DataFlowProcessor *dfPrcr) {
  m_cxt = cxt;
  m_processCxt = processCxt;
  m_writer = writer;
  m_lastCheck = 0;
  m_pfSet = processCxt->getPFSet();
  m_procEvtPrcr =
      new processevent::ProcessEventProcessor(writer, processCxt, dfPrcr);
}

ControlFlowProcessor::~ControlFlowProcessor() {
  if (m_procEvtPrcr != nullptr) {
    delete m_procEvtPrcr;
  }
}

void ControlFlowProcessor::setUID(sinsp_evt *ev) { m_procEvtPrcr->setUID(ev); }

inline void ControlFlowProcessor::processFlow(sinsp_evt *ev, OpFlags flag) {
  bool created = false;
  ProcessObj *proc = m_processCxt->getProcess(ev, SFObjectState::REUP, created);
  ProcessFlowObj *pfo = proc->pfo;
  if (pfo == nullptr) {
    // SF_INFO(m_logger, "Creating a new process flow")
    processNewFlow(ev, proc, flag);
  } else {
    updateProcFlow(pfo, flag, ev);
  }
}

inline void ControlFlowProcessor::processNewFlow(sinsp_evt *ev,
                                                 ProcessObj *proc,
                                                 OpFlags flag) {
  auto *pf = new ProcessFlowObj();
  pf->exportTime = utils::getCurrentTime(m_cxt);
  pf->lastUpdate = utils::getCurrentTime(m_cxt);
  populateProcFlow(pf, flag, ev, proc);
  updateProcFlow(pf, flag, ev);
  proc->pfo = pf;
  m_pfSet->insert(proc);
}

inline void ControlFlowProcessor::populateProcFlow(ProcessFlowObj *pf,
                                                   OpFlags flag, sinsp_evt *ev,
                                                   ProcessObj *proc) {
  pf->procflow.opFlags = flag;
  pf->procflow.ts = ev->get_ts();
  pf->procflow.endTs = 0;
  pf->procflow.procOID.hpid = proc->proc.oid.hpid;
  pf->procflow.procOID.createTS = proc->proc.oid.createTS;
  pf->procflow.numThreadsCloned = 0;
  pf->procflow.numThreadsExited = 0;
  pf->procflow.numCloneErrors = 0;
}

inline void ControlFlowProcessor::updateProcFlow(ProcessFlowObj *pf,
                                                 OpFlags flag, sinsp_evt *ev) {
  pf->procflow.opFlags |= flag;
  pf->lastUpdate = utils::getCurrentTime(m_cxt);
  if (flag == OP_CLONE) {
    int res = utils::getSyscallResult(ev);
    if (res == 0) {
      // nf->netflow.numRRecvBytes += res;
      pf->procflow.numThreadsCloned++;
    } else if (res == -1) {
      pf->procflow.numCloneErrors++;
    }
  } else if (flag == OP_EXIT) {
    pf->procflow.numThreadsExited++;
  }
}

inline void ControlFlowProcessor::removeAndWriteProcessFlow(ProcessObj *proc) {
  // SF_INFO(m_logger, "removeAndWriteProcessFlow")
  m_writer->writeProcessFlow(&((proc->pfo)->procflow));
  m_processCxt->removeProcessFromSet(proc, true);
  proc->pfo = nullptr;
}

int ControlFlowProcessor::handleProcEvent(sinsp_evt *ev, OpFlags flag) {
  sinsp_threadinfo *ti = ev->get_thread_info();

  switch (flag) {
  case OP_EXIT: {
    if (m_cxt->isProcessFlowEnabled() && !ti->is_main_thread()) {
      processFlow(ev, flag);
    }
    if (ti->is_main_thread() || !m_cxt->isProcessFlowEnabled()) {
      if (m_cxt->isProcessFlowEnabled()) {
        bool created = false;
        ProcessObj *proc =
            m_processCxt->getProcess(ev, SFObjectState::REUP, created);
        if (proc->pfo != nullptr) {
          proc->pfo->procflow.endTs = ev->get_ts();
          removeAndWriteProcessFlow(proc);
        }
      }
      m_procEvtPrcr->writeExitEvent(ev);
    }
    break;
  }
  case OP_EXEC: {
    m_procEvtPrcr->writeExecEvent(ev);
    break;
  }
  case OP_SETUID: {
    m_procEvtPrcr->writeSetUIDEvent(ev);
    break;
  }
  case OP_CLONE: {
    if (!m_cxt->isProcessFlowEnabled()) {
      m_procEvtPrcr->writeCloneEvent(ev);
    } else {
      int res = utils::getSyscallResult(ev);
      if ((ti->is_main_thread() && res == 0) ||
          (res > 0 && !utils::isCloneThreadSet(ev))) {
        m_procEvtPrcr->writeCloneEvent(ev);
      } else {
        processFlow(ev, flag);
      }
    }
    break;
  }
  default:
    SF_ERROR(m_logger, "Unsupported proc event: " << flag);
  }
  return 2;
}

void ControlFlowProcessor::exportProcessFlow(ProcessFlowObj *pfo) {
  pfo->procflow.endTs = utils::getSysdigTime(m_cxt);
  m_processCxt->exportProcess(&(pfo->procflow.procOID));
  m_writer->writeProcessFlow(&(pfo->procflow));
  SF_DEBUG(m_logger, "Reupping proc flow");
  pfo->procflow.ts = utils::getSysdigTime(m_cxt);
  pfo->procflow.endTs = 0;
  pfo->procflow.opFlags = 0;
  pfo->procflow.numThreadsCloned = 0;
  pfo->procflow.numThreadsExited = 0;
  pfo->procflow.numCloneErrors = 0;
}

void ControlFlowProcessor::printFlowStats() {
  SF_INFO(m_logger, "CF Set: " << m_pfSet->size());
}

int ControlFlowProcessor::checkForExpiredRecords() {
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
  SF_DEBUG(m_logger, "Checking expired PROC Flows!!!....");
  for (auto it = m_pfSet->begin(); it != m_pfSet->end();) {
    // SF_INFO(m_logger, "Checking flow with exportTime: " <<
    // (*it)->pfo->exportTime
    //                                                   << " Now: " << now);
    if (difftime(now, (*it)->pfo->exportTime) >= m_cxt->getNFExportInterval()) {
      SF_DEBUG(m_logger, "Exporting Proc flow!!! ");
      if (difftime(now, (*it)->pfo->lastUpdate) >=
          m_cxt->getNFExpireInterval()) {
        //	SF_INFO(m_logger, "Deleting processflow...")
        //	SF_INFO(m_logger, "NOW: " << now << " LastUpdate: " <<
        //(*it)->pfo->lastUpdate);
        delete (*it)->pfo;
        (*it)->pfo = nullptr;
        it = m_pfSet->erase(it);
      } else {
        //	SF_INFO(m_logger, "Exporting processflow...checkForExpired")
        exportProcessFlow((*it)->pfo);
        ProcessObj *p = (*it);
        it = m_pfSet->erase(it);
        p->pfo->exportTime = utils::getCurrentTime(m_cxt);
        m_pfSet->insert(p);
      }
      i++;
    } else {
      break;
    }
  }
  return i;
}

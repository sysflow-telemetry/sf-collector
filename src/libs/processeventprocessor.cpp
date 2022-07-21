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

#include "processeventprocessor.h"

using processevent::ProcessEventProcessor;

CREATE_LOGGER(ProcessEventProcessor, "sysflow.processevent");
ProcessEventProcessor::ProcessEventProcessor(
    writer::SysFlowWriter *writer, process::ProcessContext *pc,
    dataflow::DataFlowProcessor *dfPrcr) {
  m_processCxt = pc;
  m_writer = writer;
  m_dfPrcr = dfPrcr;
}

ProcessEventProcessor::~ProcessEventProcessor() = default;

void ProcessEventProcessor::setUID(sinsp_evt *ev) {
  m_uid = ev->get_param_value_str(SF_UID);
}

void ProcessEventProcessor::writeCloneEvent(sinsp_evt *ev) {
  sinsp_threadinfo *ti = ev->get_thread_info();
  bool created = false;
  ProcessObj *proc =
      m_processCxt->getProcess(ev, SFObjectState::CREATED, created);
  m_procEvt.opFlags = OP_CLONE;
  m_procEvt.ts = ev->get_ts();
  m_procEvt.procOID.hpid = proc->proc.oid.hpid;
  m_procEvt.procOID.createTS = proc->proc.oid.createTS;
  m_procEvt.tid = ti->m_tid;
  m_procEvt.ret = utils::getSyscallResult(ev);
  m_procEvt.args.clear();
  m_writer->writeProcessEvent(&m_procEvt, &(proc->proc));
}

void ProcessEventProcessor::writeSetUIDEvent(sinsp_evt *ev) {
  sinsp_threadinfo *ti = ev->get_thread_info();
  bool created = false;
  ProcessObj *proc = m_processCxt->getProcess(ev, SFObjectState::REUP, created);
  if (!created) {
    m_processCxt->updateProcess(&(proc->proc), ev, SFObjectState::MODIFIED);
    SF_DEBUG(m_logger, "Writing modified process..." << proc->proc.exe);
    m_writer->writeProcess(&(proc->proc));
    proc->written = true;
  }
  m_procEvt.opFlags = OP_SETUID;
  m_procEvt.ts = ev->get_ts();
  m_procEvt.procOID.hpid = proc->proc.oid.hpid;
  m_procEvt.procOID.createTS = proc->proc.oid.createTS;
  m_procEvt.tid = ti->m_tid;
  m_procEvt.ret = utils::getSyscallResult(ev);
  m_procEvt.args.clear();
  m_procEvt.args.push_back(m_uid);
  m_writer->writeProcessEvent(&m_procEvt, &(proc->proc));
  m_procEvt.args.clear();
}

void ProcessEventProcessor::writeExitEvent(sinsp_evt *ev) {
  sinsp_threadinfo *ti = ev->get_thread_info();
  bool created = false;
  ProcessObj *proc = m_processCxt->getProcess(ev, SFObjectState::REUP, created);
  m_procEvt.opFlags = OP_EXIT;
  m_procEvt.ts = ev->get_ts();
  m_procEvt.procOID.hpid = proc->proc.oid.hpid;
  m_procEvt.procOID.createTS = proc->proc.oid.createTS;
  m_procEvt.tid = ti->m_tid;
  m_procEvt.ret = utils::getSyscallResult(ev);
  m_procEvt.args.clear();
  int64_t tid = -1;
  if (!ti->is_main_thread()) {
    tid = ti->m_tid;
  }
  m_dfPrcr->removeAndWriteDFFromProc(proc, tid);
  m_writer->writeProcessEvent(&m_procEvt, &(proc->proc));
  // delete the process from the proc table after an exit
  if (ti->is_main_thread()) {
    // m_processCxt->deleteProcess(&proc);
    m_processCxt->markForDeletion(&proc);
  }
}

void ProcessEventProcessor::writeExecEvent(sinsp_evt *ev) {
  sinsp_threadinfo *ti = ev->get_thread_info();
  bool created = false;
  ProcessObj *proc =
      m_processCxt->getProcess(ev, SFObjectState::CREATED, created);

  // If Clones are filtered out (or a process like bash is filtered out) then we
  // will only see the EXEC of this process, and the getProcess above will
  // actually create it.  So the question is do we want to add another process
  // record just to mark it modified at this point?
  if (!created) {
    m_processCxt->updateProcess(&(proc->proc), ev, SFObjectState::MODIFIED);
    SF_DEBUG(m_logger, "Writing modified process..." << proc->proc.exe);
    m_writer->writeProcess(&(proc->proc));
    proc->written = true;
  }

  m_procEvt.opFlags = OP_EXEC;
  m_procEvt.ts = ev->get_ts();
  m_procEvt.procOID.hpid = proc->proc.oid.hpid;
  m_procEvt.procOID.createTS = proc->proc.oid.createTS;
  m_procEvt.tid = ti->m_tid;
  m_procEvt.ret = utils::getSyscallResult(ev);
  m_procEvt.args.clear();
  m_writer->writeProcessEvent(&m_procEvt, &(proc->proc));
}

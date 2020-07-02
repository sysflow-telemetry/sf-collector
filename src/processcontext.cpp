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

#include "processcontext.h"

using process::ProcessContext;

CREATE_LOGGER(ProcessContext, "sysflow.process");
ProcessContext::ProcessContext(context::SysFlowContext *cxt,
                               container::ContainerContext *ccxt,
                               file::FileContext *fileCxt,
                               writer::SysFlowWriter *writer)
    : m_procs(PROC_TABLE_SIZE), m_delProcQue() {
  m_cxt = cxt;
  OID *emptyoidkey = utils::getOIDEmptyKey();
  OID *deloidkey = utils::getOIDDelKey();
  m_delProcTime = utils::getCurrentTime(m_cxt);
  m_procs.set_empty_key(emptyoidkey);
  m_procs.set_deleted_key(deloidkey);
  m_containerCxt = ccxt;
  m_writer = writer;
  m_fileCxt = fileCxt;
}

ProcessContext::~ProcessContext() {
  try {
    clearAllProcesses();
  } catch (...) {
    SF_ERROR(m_logger, "Caught exception while clearing all process in process "
                       "context destructor");
  }
}

ProcessFlowSet *ProcessContext::getPFSet() { return &m_pfSet; }

ProcessObj *ProcessContext::createProcess(api::SysFlowProcess *pr,
                                          api::SysFlowEvent *ev,
                                          SFObjectState state) {
  auto *p = new ProcessObj();
  p->proc.state = state;
  p->proc.ts = ev->getTS();
  p->proc.oid.hpid = pr->getHostPID();
  p->proc.oid.createTS = pr->getCreateTS();
  p->proc.tty = pr->getTTY();
  // auto *inspector = m_cxt->getInspector();
  // auto *parent = inspector->getProcess(pr->getParentPID());

  if (pr->hasParent()) {
    OID poid;
    poid.createTS = pr->getParentCreateTS();
    poid.hpid = pr->getParentPID();
    p->proc.poid.set_OID(poid);
    SF_DEBUG(m_logger, "PARENT PID: " << pr->getParentPID() << " ts "
                                      << pr->getParentCreateTS())
  } else {
    SF_DEBUG(m_logger, "Unable to find parent for process "
                           << pr->getExe() << " PID " << pr->getHostPID()
                           << " PTID " << pr->getParentPID())
    if (pr->getParentPID() != -1) {
      SF_DEBUG(m_logger, "Searching for potential parent in process table.")
      ProcessObj *pt = getProcess(pr->getParentPID());
      if (pt != nullptr) {
        SF_DEBUG(m_logger, "Found parent candidate! Process: " << pt->proc.exe);
        OID poid;
        poid.createTS = pt->proc.oid.createTS;
        poid.hpid = pt->proc.oid.hpid;
        p->proc.poid.set_OID(poid);
      }
    }
  }
  p->proc.exe = pr->getExe();

  SF_DEBUG(m_logger, "createProcess: The exepath is "
                         << p->proc.exe << "pr->getExe: " << pr->getExe());
  // p->proc.exeArgs.clear();
  p->proc.exeArgs = pr->getArgs();
  /*int i = 0;
  for (auto it = mainthread->m_args.begin(); it != mainthread->m_args.end();
       ++it) {
    if (i == 0) {
      p->proc.exeArgs += *it;
    } else {
      p->proc.exeArgs += " " + *it;
    }
    i++;
  }*/
  p->proc.uid = pr->getUID();
  p->proc.gid = pr->getGID();
  p->proc.userName = pr->getUserName();
  p->proc.groupName = pr->getGroupName();
  ContainerObj *cont = m_containerCxt->getContainer(pr);
  if (cont != nullptr) {
    p->proc.containerId.set_string(cont->cont.id);
    cont->refs++;
  } else {
    p->proc.containerId.set_null();
  }
  return p;
}

void ProcessContext::printAncestors(Process *proc) {
  Process::poid_t poid = proc->poid;

  while (!poid.is_null()) {
    OID key = poid.get_OID();
    ProcessTable::iterator p = m_procs.find(&(key));
    if (p != m_procs.end()) {
      poid = p->second->proc.poid;
      SF_INFO(m_logger, "-->" << p->second->proc.oid.hpid << " "
                              << p->second->proc.oid.createTS << " "
                              << p->second->proc.exe << " "
                              << p->second->proc.exeArgs);
    } else {
      break;
    }
  }
}

bool ProcessContext::isAncestor(OID *oid, Process *proc) {

  Process::poid_t poid = proc->poid;

  while (!poid.is_null()) {
    OID key = poid.get_OID();
    ProcessTable::iterator p = m_procs.find(&(key));
    if (p != m_procs.end()) {
      OID o = p->second->proc.oid;
      if (oid->hpid == o.hpid && oid->createTS == o.createTS) {
        return true;
      }
      poid = p->second->proc.poid;
    } else {
      break;
    }
  }

  return false;
}

void ProcessContext::reupContainer(api::SysFlowProcess *pr, ProcessObj *proc) {
  string containerId = "";
  if (!proc->proc.containerId.is_null()) {
    containerId = proc->proc.containerId.get_string();
  }
  ContainerObj *cont1 = nullptr;
  if (!containerId.empty()) {
    cont1 = m_containerCxt->getContainer(containerId);
    if (cont1 != nullptr) {
      cont1->refs--;
    }
  }
  ContainerObj *cont = m_containerCxt->getContainer(pr);
  if (cont != nullptr) {
    proc->proc.containerId.set_string(cont->cont.id);
    cont->refs++;
  } else {
    proc->proc.containerId.set_null();
  }
}

ProcessObj *ProcessContext::getProcess(int64_t pid) {
  // SF_DEBUG(m_logger, "getProcess PID: " << pid)
  ProcessTable::iterator it;
  // SF_DEBUG(m_logger, " Size of Proc Table: " << m_procs.size())
  for (it = m_procs.begin(); it != m_procs.end(); it++) {
    // SF_DEBUG(m_logger, "getProcess Iterating PID: " << it->first->hpid)
    if (it->first->hpid == pid) {
      // SF_DEBUG(m_logger, "getProcess FOUND/RETURNING PID: " << pid)
      return it->second;
    }
  }

  // SF_DEBUG(m_logger, "getProcess RETURN NULL")
  return nullptr;
}

ProcessObj *ProcessContext::getProcess(api::SysFlowEvent *ev,
                                       SFObjectState state, bool &created) {
  OID key;
  auto pr = ev->getProcess();
  auto inspector = m_cxt->getInspector();
  key.createTS = pr->getCreateTS();
  key.hpid = pr->getHostPID();
  created = true;
  SF_DEBUG(m_logger, "getProcess: PID: " << pr->getHostPID() << " ts "
                                         << pr->getCreateTS()
                                         << " EXEPATH: " << pr->getExe());
  ProcessTable::iterator proc = m_procs.find(&key);
  ProcessObj *process = nullptr;
  if (proc != m_procs.end()) {
    created = false;
    if (proc->second->written) {
      return proc->second;
    }
    process = proc->second;
    process->proc.state = SFObjectState::REUP;
  }
  std::vector<ProcessObj *> processes;
  if (process == nullptr) {
    process = createProcess(pr, ev, state);
  } else { // must make sure the container is in the sysflow file..
    reupContainer(pr, process);
  }
  SF_DEBUG(m_logger, "CREATING PROCESS FOR WRITING: PID: "
                         << pr->getHostPID() << " ts " << pr->getCreateTS()
                         << " EXEPATH: " << pr->getExe());
  processes.push_back(process);

  std::unique_ptr<api::SysFlowProcess> ct =
      inspector->getProcess(pr->getHostPID());
  std::unique_ptr<api::SysFlowProcess> mt =
      inspector->getProcess(pr->getParentPID());
  int parentPID = -1;
  while (mt != nullptr) {
    if (mt->getCreateTS() == 0 && mt->getHostPID() == 0) {
      parentPID = mt->getParentPID();
      ct = std::move(mt);
      mt = inspector->getProcess(parentPID);
      continue;
    }
    key.createTS = mt->getCreateTS();
    key.hpid = mt->getHostPID();
    SF_DEBUG(m_logger, "PARENT PID: " << mt->getHostPID() << " ts "
                                      << mt->getCreateTS()
                                      << " EXEPATH: " << mt->getExe());
    ProcessObj *parent = nullptr;
    ProcessTable::iterator proc2 = m_procs.find(&key);
    if (proc2 != m_procs.end()) {
      if (proc2->second->written) {
        break;
      } else {
        parent = proc2->second;
        parent->proc.state = SFObjectState::REUP;
      }
    }
    if (parent == nullptr) {
      parent = createProcess(mt.get(), ev, SFObjectState::REUP);
    } else {
      reupContainer(mt.get(), parent);
    }
    parent->children.insert(processes.back()->proc.oid);
    processes.push_back(parent);
    parentPID = mt->getParentPID();
    ct = std::move(mt);
    mt = inspector->getProcess(parentPID);
  }
  if (mt == nullptr && ct->getParentPID() != -1) {
    SF_DEBUG(m_logger,
             "Ancestral chain not found standard way.. searching based on pid")
    ProcessObj *prt = getProcess(ct->getParentPID());
    OID o;
    o.hpid = ct->getHostPID();
    o.createTS = ct->getCreateTS();
    while (prt != nullptr) {
      SF_DEBUG(m_logger, "Found parent with pid: " << prt->proc.oid.hpid
                                                   << " create TS "
                                                   << prt->proc.oid.createTS
                                                   << " exe: " << prt->proc.exe)
      if (!prt->written) {
        SF_DEBUG(m_logger, "Writing to process vector...")
        processes.push_back(prt);
      }
      prt->children.insert(o);
      o.hpid = prt->proc.oid.hpid;
      o.createTS = prt->proc.oid.createTS;
      if (prt->proc.poid.is_null()) {
        break;
      }
      OID poid = prt->proc.poid.get_OID();
      prt = getProcess(&poid);
    }
  }

  SF_DEBUG(m_logger, " Size of Proc Table: " << m_procs.size())
  for (auto it = processes.rbegin(); it != processes.rend(); ++it) {
    SF_DEBUG(m_logger, "Writing process " << (*it)->proc.exe << " "
                                          << (*it)->proc.oid.hpid);
    m_procs[&((*it)->proc.oid)] = (*it);
    m_writer->writeProcess(&((*it)->proc));
    (*it)->written = true;
  }
  SF_DEBUG(m_logger, " Size of Proc Table: " << m_procs.size())
  return process;
}

ProcessObj *ProcessContext::getProcess(OID *oid) {
  ProcessTable::iterator proc = m_procs.find(oid);
  if (proc != m_procs.end()) {
    return proc->second;
  }
  return nullptr;
}

bool ProcessContext::exportProcess(OID *oid) {
  ProcessObj *p = getProcess(oid);
  bool expt = false;
  if (p == nullptr) {
    SF_ERROR(m_logger, "Can't find process to export! oid: " << oid->hpid << " "
                                                             << oid->createTS);
    return expt;
  }
  if (!p->proc.containerId.is_null()) {
    m_containerCxt->exportContainer(p->proc.containerId.get_string());
  }
  if (!p->written) {
    m_writer->writeProcess(&(p->proc));
    p->written = true;
    expt = true;
  }
  return expt;
}

void ProcessContext::updateProcess(Process *proc, api::SysFlowEvent *ev,
                                   SFObjectState state) {
  auto pr = ev->getProcess();
  proc->state = state;
  proc->ts = ev->getTS();
  proc->exe = pr->getExe();
  proc->exeArgs = pr->getArgs();
  proc->uid = pr->getUID();
  proc->gid = pr->getGID();
  proc->userName = pr->getUserName();
  proc->groupName = pr->getGroupName();
}

void ProcessContext::clearProcesses() {
  for (ProcessTable::iterator it = m_procs.begin(); it != m_procs.end(); ++it) {
    if (it->second->netflows.empty() && it->second->fileflows.empty() &&
        it->second->children.empty() && it->second->pfo == nullptr) {
      ProcessObj *proc = it->second;
      Process::poid_t poid = proc->proc.poid;
      ProcessObj *curProc = proc;

      while (!poid.is_null()) {
        OID key = poid.get_OID();
        ProcessTable::iterator p = m_procs.find(&key);
        if (p != m_procs.end()) {
          ProcessObj *parentProc = p->second;
          parentProc->children.erase(curProc->proc.oid);
          if (parentProc->children.empty() && parentProc->netflows.empty() &&
              parentProc->fileflows.empty()) {
            curProc = parentProc;
            poid = curProc->proc.poid;
          } else {
            break;
          }

        } else {
          break;
        }
      }
      if (!proc->proc.containerId.is_null()) {
        m_containerCxt->derefContainer(proc->proc.containerId.get_string());
      }
      m_procs.erase(it);
      delete proc;
    } else {
      it->second->written = false;
    }
  }
  for (ProcessTable::iterator it = m_procs.begin(); it != m_procs.end(); ++it) {
    if (it->second->netflows.empty() && it->second->fileflows.empty() &&
        it->second->children.empty() && it->second->pfo == nullptr) {
      ProcessObj *proc = it->second;
      if (!proc->proc.containerId.is_null()) {
        m_containerCxt->derefContainer(proc->proc.containerId.get_string());
      }
      m_procs.erase(it);
      delete proc;
    }
  }
}

void ProcessContext::printStats() {
  SF_INFO(m_logger, "# Containers: " << m_containerCxt->getSize()
                                     << " # Procs: " << m_procs.size());
  for (ProcessTable::iterator it = m_procs.begin(); it != m_procs.end(); ++it) {
    ProcessObj *proc = it->second;
    SF_INFO(m_logger, proc->proc.exe << " " << proc->children.size() << " "
                                     << proc->netflows.size() << " "
                                     << proc->fileflows.size());
  }
}

void ProcessContext::writeProcessAndAncestors(ProcessObj *proc) {
  Process::poid_t poid = proc->proc.poid;
  std::vector<ProcessObj *> processes;
  processes.push_back(proc);

  while (!poid.is_null()) {
    OID key = poid.get_OID();
    ProcessTable::iterator p = m_procs.find(&key);
    if (p != m_procs.end()) {
      if (!p->second->written) {
        processes.push_back(p->second);
      }
      poid = p->second->proc.poid;
    } else {
      break;
    }
  }
  for (auto it = processes.rbegin(); it != processes.rend(); ++it) {
    SF_DEBUG(m_logger, "Final: writing process " << (*it)->proc.exe << " "
                                                 << (*it)->proc.oid.hpid);
    ProcessObj *proc = (*it);
    if (!proc->proc.containerId.is_null()) {
      m_containerCxt->exportContainer(proc->proc.containerId.get_string());
    }
    m_writer->writeProcess(&((*it)->proc));
    (*it)->written = true;
  }
}

void ProcessContext::printNetworkFlow(ProcessObj *proc) {

  for (NetworkFlowTable::iterator it = proc->netflows.begin();
       it != proc->netflows.end(); it++) {
    cout << "Netflow: " << it->first.ip1 << " " << it->first.ip2 << " "
         << it->first.port1 << " " << it->first.port2 << " " << it->first.tid
         << " " << it->first.fd << endl;
  }
}

void ProcessContext::clearAllProcesses() {
  SF_INFO(m_logger, "Deleting all processes");
  for (ProcessTable::iterator it = m_procs.begin(); it != m_procs.end(); ++it) {
    if (((!it->second->netflows.empty()) || (!it->second->fileflows.empty()) ||
         (it->second->pfo != nullptr)) &&
        !it->second->written) {
      writeProcessAndAncestors(it->second);
    }
    for (NetworkFlowTable::iterator nfi = it->second->netflows.begin();
         nfi != it->second->netflows.end(); nfi++) {
      nfi->second->netflow.opFlags |= OP_TRUNCATE;
      nfi->second->netflow.endTs = utils::getSystemTime(m_cxt);
      m_writer->writeNetFlow(&(nfi->second->netflow));
      delete nfi->second;
    }
    for (FileFlowTable::iterator ffi = it->second->fileflows.begin();
         ffi != it->second->fileflows.end(); ffi++) {
      ffi->second->fileflow.opFlags |= OP_TRUNCATE;
      ffi->second->fileflow.endTs = utils::getSystemTime(m_cxt);
      m_fileCxt->exportFile(ffi->second->filekey);
      m_writer->writeFileFlow(&(ffi->second->fileflow));
      delete ffi->second;
    }
    if (it->second->pfo != nullptr) {
      it->second->pfo->procflow.opFlags |= OP_TRUNCATE;
      it->second->pfo->procflow.endTs = utils::getSystemTime(m_cxt);
      SF_INFO(m_logger, "Writing processflow!")
      m_writer->writeProcessFlow(&(it->second->pfo->procflow));
      delete it->second->pfo;
      it->second->pfo = nullptr;
    }
  }

  for (ProcessTable::iterator it = m_procs.begin(); it != m_procs.end(); ++it) {
    delete it->second;
  }

  for (auto it = m_delProcQue.begin(); it != m_delProcQue.end(); ++it) {
    delete (*it);
    it = m_delProcQue.erase(it);
  }
}

void ProcessContext::markForDeletion(ProcessObj **proc) {
  OIDObj *o = new OIDObj((*proc)->proc.oid);
  o->exportTime = utils::getCurrentTime(m_cxt);
  m_delProcQue.push_back(o);
  *proc = nullptr;
}

void ProcessContext::deleteProcess(ProcessObj **proc) {
  Process::poid_t poid = (*proc)->proc.poid;
  if (!poid.is_null()) {
    OID key = poid.get_OID();
    ProcessTable::iterator p = m_procs.find(&key);
    if (p != m_procs.end()) {
      p->second->children.erase((*proc)->proc.oid);
    }
  }
  if (!(*proc)->proc.containerId.is_null()) {
    m_containerCxt->derefContainer((*proc)->proc.containerId.get_string());
  }
  if ((*proc)->pfo != nullptr) {
    removeProcessFromSet(*proc, false);
  }
  m_procs.erase(&((*proc)->proc.oid));
  delete *proc;
  *proc = nullptr;
}

int ProcessContext::removeProcessFromSet(ProcessObj *proc, bool checkForErr) {
  bool found = false;
  int removed = 0;
  for (auto iter = m_pfSet.find(proc); iter != m_pfSet.end(); iter++) {
    auto *foundObj = static_cast<ProcessObj *>(*iter);
    if (*foundObj == *proc) {
      SF_INFO(m_logger, "Removing procflow element from multiset.");
      m_pfSet.erase(iter);
      removed++;
      found = true;
      break;
    }
  }
  if (!found && checkForErr) {
    SF_ERROR(m_logger,
             "Cannot find Procflow Object in proc flow set. Deleting. "
             "This should not happen");
  }
  if (proc->pfo != nullptr) {
    delete proc->pfo;
    proc->pfo = nullptr;
  }
  return removed;
}

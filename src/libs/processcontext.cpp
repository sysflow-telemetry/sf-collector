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

ProcessObj *ProcessContext::createProcess(sinsp_threadinfo *ti, sinsp_evt *ev,
                                          SFObjectState state) {
  auto *p = new ProcessObj();
  sinsp_threadinfo *mainthread =
      ti->is_main_thread() ? ti : ti->get_main_thread();
  p->proc.state = state;
  p->proc.ts = ev->get_ts();
  p->proc.oid.hpid = mainthread->m_pid;
  p->proc.oid.createTS = mainthread->m_clone_ts;
  p->proc.entry = (mainthread->m_vpid == 1);
  p->proc.poid.set_null();

  p->proc.tty = mainthread->m_tty;
  sinsp_threadinfo *parent = mainthread->get_parent_thread();

  if (parent != nullptr) {
    OID poid;
    parent = parent->is_main_thread() ? parent : parent->get_main_thread();
    poid.createTS = parent->m_clone_ts;
    poid.hpid = parent->m_pid;
    p->proc.poid.set_OID(poid);
    SF_DEBUG(m_logger,
             "PARENT PID: " << parent->m_pid << " ts " << parent->m_clone_ts)
    SF_DEBUG(m_logger, "Creating Process: " << mainthread->m_pid << " "
                                            << mainthread->m_clone_ts << " "
                                            << parent->m_pid << " "
                                            << parent->m_clone_ts);
  } else {
    SF_DEBUG(m_logger, "Unable to find parent for process "
                           << mainthread->m_exe << " PID " << mainthread->m_pid
                           << " PTID " << mainthread->m_ptid)
    if (mainthread->m_ptid != -1) {
      SF_DEBUG(m_logger, "Searching for potential parent in process table.")
      ProcessObj *pt = getProcess(mainthread->m_ptid);
      if (pt != nullptr) {
        SF_DEBUG(m_logger, "Found parent candidate! Process: " << pt->proc.exe);
        OID poid;
        poid.createTS = pt->proc.oid.createTS;
        poid.hpid = pt->proc.oid.hpid;
        p->proc.poid.set_OID(poid);
      }
    }
  }
<<<<<<< HEAD
  // this is a fallback to retrieve the process name when exepath is <NA>, which
  // can happen when a page fault occurs during an execve system call.
=======
>>>>>>> 0ad6fbc (fix(processcontext): partial fix for <NA> issue for processes)
  p->proc.exe = (mainthread->m_exepath.empty() ||
                 mainthread->m_exepath.compare("<NA>") == 0)
                    ? mainthread->m_exe
                    : mainthread->m_exepath;
  SF_DEBUG(m_logger, "createProcess: The exepath is "
                         << p->proc.exe
                         << " mt->exepath: " << mainthread->get_exepath()
                         << " EXE: " << mainthread->get_exe()
                         << " CWD: " << mainthread->get_cwd());
  p->proc.exeArgs.clear();
  int i = 0;
  for (auto it = mainthread->m_args.begin(); it != mainthread->m_args.end();
       ++it) {
    if (i == 0) {
      p->proc.exeArgs += *it;
    } else {
      p->proc.exeArgs += " " + *it;
    }
    i++;
  }
  p->proc.uid = mainthread->m_user.uid;
  p->proc.gid = mainthread->m_group.gid;
  p->proc.userName = mainthread->m_user.name;
  p->proc.groupName = mainthread->m_group.name;
  ContainerObj *cont = m_containerCxt->getContainer(ti);
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
      SF_DEBUG(m_logger, "-->" << p->second->proc.oid.hpid << " "
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

void ProcessContext::reupContainer(sinsp_threadinfo *ti, ProcessObj *proc) {
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
  ContainerObj *cont = m_containerCxt->getContainer(ti);
  if (cont != nullptr) {
    proc->proc.containerId.set_string(cont->cont.id);
    cont->refs++;
  } else {
    proc->proc.containerId.set_null();
  }
}

ProcessObj *ProcessContext::getProcess(int64_t pid) {
  ProcessTable::iterator it;

  for (it = m_procs.begin(); it != m_procs.end(); it++) {
    if (it->first->hpid == pid) {
      return it->second;
    }
  }

  return nullptr;
}

ProcessObj *ProcessContext::getProcess(sinsp_evt *ev, SFObjectState state,
                                       bool &created) {
  sinsp_threadinfo *ti = ev->get_thread_info();
  sinsp_threadinfo *mt = ti->get_main_thread();
  OID key;
  key.createTS = mt->m_clone_ts;
  key.hpid = mt->m_pid;
  created = true;

  SF_DEBUG(m_logger,
           "Get process - PID: " << mt->m_pid << " ts: " << mt->m_clone_ts
                                 << " Exepath: " << mt->m_exepath << " Exe: "
                                 << mt->m_exe << " MTCI " << mt->m_container_id
                                 << " TICI: " << ti->m_container_id)
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
    // use the curretn thread here rather than the main thread because it
    // appears the main thread does not always get the container id right away.
    process = createProcess(ti, ev, state);
  } else { // must make sure the container is in the sysflow file..
    reupContainer(ti, process);
  }

  SF_DEBUG(m_logger, "Creating process for writing - PID: "
                         << mt->m_pid << " ts: " << mt->m_clone_ts
                         << " Exepath: " << mt->m_exepath
                         << " Exe: " << mt->m_exe);
  processes.push_back(process);

  sinsp_threadinfo *ct = mt;
  mt = mt->get_parent_thread();

  while (mt != nullptr && mt->m_tid != -1) {
    mt = mt->is_main_thread() ? mt : mt->get_main_thread();
    if (mt->m_clone_ts == 0 && mt->m_pid == 0) {
      ct = mt;
      mt = mt->get_parent_thread();
      continue;
    }
    key.createTS = mt->m_clone_ts;
    key.hpid = mt->m_pid;

    SF_DEBUG(m_logger, "Parent - PID: " << mt->m_pid
                                        << " ts: " << mt->m_clone_ts
                                        << " Exepath: " << mt->m_exepath
                                        << " Exe: " << mt->m_exe)
    ProcessObj *parent = nullptr;
    ProcessTable::iterator proc2 = m_procs.find(&key);
    if (proc2 != m_procs.end()) {
      SF_DEBUG(m_logger, "Found parent - PID: " << mt->m_pid
                                                << " ts: " << mt->m_clone_ts
                                                << " Exepath: " << mt->m_exepath
                                                << " Exe: " << mt->m_exe)
      if (proc2->second->written) {
        break;
      } else {
        parent = proc2->second;
        parent->proc.state = SFObjectState::REUP;
      }
    }

    if (parent == nullptr) {
      SF_DEBUG(m_logger, "Creating parent - PID: "
                             << mt->m_pid << " ts: " << mt->m_clone_ts
                             << " EXEPATH: " << mt->m_exepath << " EXE: "
                             << mt->m_exe << " " << mt->is_main_thread())
      parent = createProcess(mt, ev, SFObjectState::REUP);
    } else {
      reupContainer(mt, parent);
    }

    parent->children.insert(processes.back()->proc.oid);
    processes.push_back(parent);
    ct = mt;
    mt = mt->get_parent_thread();
  }

  if (mt == nullptr && ct->m_ptid != -1) {
    SF_DEBUG(m_logger, "Ancestral chain not found through standard way. "
                       "Searching based on pid.")
    ProcessObj *prt = getProcess(ct->m_ptid);
    OID o;
    o.hpid = ct->m_pid;
    o.createTS = ct->m_clone_ts;

    while (prt != nullptr) {
      SF_DEBUG(m_logger, "Found parent with PID: " << prt->proc.oid.hpid
                                                   << " Create TS "
                                                   << prt->proc.oid.createTS
                                                   << " Exe: " << prt->proc.exe)
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

  SF_DEBUG(m_logger, "Size of process table: " << m_procs.size())
  for (auto it = processes.rbegin(); it != processes.rend(); ++it) {
    SF_DEBUG(m_logger, "Writing process " << (*it)->proc.exe << " "
                                          << (*it)->proc.oid.hpid);
    m_procs[&((*it)->proc.oid)] = (*it);
    m_writer->writeProcess(&((*it)->proc));
    (*it)->written = true;
  }

  SF_DEBUG(m_logger, "New size of process table: " << m_procs.size())
  return process;
}

ProcessObj *ProcessContext::getProcess(OID *oid) {
  ProcessTable::iterator proc = m_procs.find(oid);
  if (proc != m_procs.end()) {
    return proc->second;
  }
  return nullptr;
}

ProcessObj *ProcessContext::exportProcess(OID *oid) {
  ProcessObj *p = getProcess(oid);
  if (p == nullptr) {
    SF_ERROR(m_logger, "Can't find process to export! OID: " << oid->hpid << " "
                                                             << oid->createTS);
    return p;
  }

  if (!p->proc.containerId.is_null()) {
    m_containerCxt->exportContainer(p->proc.containerId.get_string());
  }

  if (!p->written) {
    m_writer->writeProcess(&(p->proc));
    p->written = true;
  }

  return p;
}

void ProcessContext::updateProcess(Process *proc, sinsp_evt *ev,
                                   SFObjectState state) {
  sinsp_threadinfo *ti = ev->get_thread_info();
  sinsp_threadinfo *mainthread = ti->get_main_thread();
  proc->state = state;
  proc->ts = ev->get_ts();
<<<<<<< HEAD
  // this is a fallback to retrieve the process name when exepath is <NA>, which
  // can happen when a page fault occurs during an execve system call.
=======
>>>>>>> 0ad6fbc (fix(processcontext): partial fix for <NA> issue for processes)
  proc->exe = (mainthread->m_exepath.empty() ||
               mainthread->m_exepath.compare("<NA>") == 0)
                  ? mainthread->m_exe
                  : mainthread->m_exepath;
  proc->exeArgs.clear();

  int i = 0;
  for (auto it = mainthread->m_args.begin(); it != mainthread->m_args.end();
       ++it) {
    if (i == 0) {
      proc->exeArgs += *it;
    } else {
      proc->exeArgs += " " + *it;
    }
    i++;
  }

  proc->uid = mainthread->m_user.uid;
  proc->gid = mainthread->m_group.gid;
  proc->userName = mainthread->m_user.name;
  proc->groupName = mainthread->m_group.name;
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
              parentProc->fileflows.empty() && parentProc->pfo == nullptr) {
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
  SF_DEBUG(m_logger, "# Containers: " << m_containerCxt->getSize()
                                      << " # Procs: " << m_procs.size());
  for (ProcessTable::iterator it = m_procs.begin(); it != m_procs.end(); ++it) {
    ProcessObj *proc = it->second;
    SF_DEBUG(m_logger, proc->proc.exe << " " << proc->children.size() << " "
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
    SF_DEBUG(m_logger, "Netflow: " << it->first.ip1 << " " << it->first.ip2
                                   << " " << it->first.port1 << " "
                                   << it->first.port2 << " " << it->first.tid
                                   << " " << it->first.fd)
  }
}

void ProcessContext::clearAllProcesses() {
  for (ProcessTable::iterator it = m_procs.begin(); it != m_procs.end(); ++it) {
    if (((!it->second->netflows.empty()) || (!it->second->fileflows.empty()) ||
         (it->second->pfo != nullptr)) &&
        !it->second->written) {
      writeProcessAndAncestors(it->second);
    }

    for (NetworkFlowTable::iterator nfi = it->second->netflows.begin();
         nfi != it->second->netflows.end(); nfi++) {
      nfi->second->netflow.opFlags |= OP_TRUNCATE;
      nfi->second->netflow.endTs = utils::getSysdigTime(m_cxt);
      m_writer->writeNetFlow(&(nfi->second->netflow), &(it->second->proc));
      delete nfi->second;
    }

    for (FileFlowTable::iterator ffi = it->second->fileflows.begin();
         ffi != it->second->fileflows.end(); ffi++) {
      ffi->second->fileflow.opFlags |= OP_TRUNCATE;
      ffi->second->fileflow.endTs = utils::getSysdigTime(m_cxt);
      FileObj *file = m_fileCxt->exportFile(ffi->second->filekey);
      m_writer->writeFileFlow(&(ffi->second->fileflow), &(it->second->proc),
                              &(file->file));
      delete ffi->second;
    }

    if (it->second->pfo != nullptr) {
      it->second->pfo->procflow.opFlags |= OP_TRUNCATE;
      it->second->pfo->procflow.endTs = utils::getSysdigTime(m_cxt);
      SF_DEBUG(m_logger, "Writing processflow")
      m_writer->writeProcessFlow(&(it->second->pfo->procflow),
                                 &(it->second->proc));
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
      SF_DEBUG(m_logger, "Removing procflow element from multiset.");
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

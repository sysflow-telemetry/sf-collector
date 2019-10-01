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
    : m_procs(PROC_TABLE_SIZE) {
  OID *emptyoidkey = utils::getOIDEmptyKey();
  OID *deloidkey = utils::getOIDDelKey();
  m_procs.set_empty_key(emptyoidkey);
  m_procs.set_deleted_key(deloidkey);
  m_cxt = cxt;
  m_containerCxt = ccxt;
  m_writer = writer;
  m_fileCxt = fileCxt;
}

ProcessContext::~ProcessContext() { clearAllProcesses(); }

ProcessObj *ProcessContext::createProcess(sinsp_threadinfo *mainthread,
                                          sinsp_evt *ev, SFObjectState state) {

  auto *p = new ProcessObj();
  p->proc.state = state;
  p->proc.ts = ev->get_ts();
  p->proc.oid.hpid = mainthread->m_pid;
  p->proc.oid.createTS = mainthread->m_clone_ts;

  sinsp_threadinfo *ti = ev->get_thread_info();
  p->proc.tty = ti->m_tty;
  sinsp_threadinfo *parent = mainthread->get_parent_thread();

  if (parent != nullptr) {
    OID poid;
    poid.createTS = parent->m_clone_ts;
    poid.hpid = parent->m_pid;
    p->proc.poid.set_OID(poid);
  }
  p->proc.exe = (mainthread->m_exepath.empty())
                    ? utils::getAbsolutePath(ti, mainthread->m_exe)
                    : mainthread->m_exepath;
  SF_DEBUG(m_logger, "createProcess: The exepath is "
                         << p->proc.exe << " ti->exepath: " << ti->get_exepath()
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
  p->proc.uid = mainthread->m_uid;
  p->proc.gid = mainthread->m_gid;
  p->proc.userName = utils::getUserName(m_cxt, mainthread->m_uid);
  p->proc.groupName = utils::getGroupName(m_cxt, mainthread->m_gid);
  ContainerObj *cont = m_containerCxt->getContainer(ev);
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

void ProcessContext::reupContainer(sinsp_evt *ev, ProcessObj *proc) {
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
  ContainerObj *cont = m_containerCxt->getContainer(ev);
  if (cont != nullptr) {
    proc->proc.containerId.set_string(cont->cont.id);
    cont->refs++;
  } else {
    proc->proc.containerId.set_null();
  }
}

ProcessObj *ProcessContext::getProcess(sinsp_evt *ev, SFObjectState state,
                                       bool &created) {
  sinsp_threadinfo *ti = ev->get_thread_info();
  sinsp_threadinfo *mt = ti->get_main_thread();
  OID key;
  key.createTS = mt->m_clone_ts;
  key.hpid = mt->m_pid;
  created = true;
  SF_DEBUG(m_logger, "getProcess: PID: " << mt->m_pid << " ts "
                                         << mt->m_clone_ts
                                         << " EXEPATH: " << mt->m_exepath
                                         << " EXE: " << mt->m_exe);
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
    process = createProcess(mt, ev, state);
  } else { // must make sure the container is in the sysflow file..
    reupContainer(ev, process);
  }
  SF_DEBUG(m_logger, "CREATING PROCESS FOR WRITING: PID: "
                         << mt->m_pid << " ts " << mt->m_clone_ts
                         << " EXEPATH: " << mt->m_exepath
                         << " EXE: " << mt->m_exe);
  processes.push_back(process);
  mt = mt->get_parent_thread();
  while (mt != nullptr) {
    if (mt->m_clone_ts == 0 && mt->m_pid == 0) {
      mt = mt->get_parent_thread();
      continue;
    }
    key.createTS = mt->m_clone_ts;
    key.hpid = mt->m_pid;
    SF_DEBUG(m_logger, "PARENT PID: " << mt->m_pid << " ts " << mt->m_clone_ts
                                      << " EXEPATH: " << mt->m_exepath
                                      << " EXE: " << mt->m_exe);
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
      parent = createProcess(mt, ev, SFObjectState::REUP);
    } else {
      reupContainer(ev, parent);
    }
    parent->children.insert(processes.back()->proc.oid);
    processes.push_back(parent);
    mt = mt->get_parent_thread();
  }

  for (auto it = processes.rbegin(); it != processes.rend(); ++it) {
    SF_DEBUG(m_logger, "Writing process " << (*it)->proc.exe << " "
                                          << (*it)->proc.oid.hpid);
    m_procs[&((*it)->proc.oid)] = (*it);
    m_writer->writeProcess(&((*it)->proc));
    (*it)->written = true;
  }
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

void ProcessContext::updateProcess(Process *proc, sinsp_evt *ev,
                                   SFObjectState state) {
  sinsp_threadinfo *ti = ev->get_thread_info();
  sinsp_threadinfo *mainthread = ti->get_main_thread();
  proc->state = state;
  proc->ts = ev->get_ts();
  proc->exe = (mainthread->m_exepath.empty())
                  ? utils::getAbsolutePath(ti, mainthread->m_exe)
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
  proc->uid = mainthread->m_uid;
  proc->gid = mainthread->m_gid;
  proc->userName = utils::getUserName(m_cxt, mainthread->m_uid);
  proc->groupName = utils::getGroupName(m_cxt, mainthread->m_gid);
}

void ProcessContext::clearProcesses() {
  for (ProcessTable::iterator it = m_procs.begin(); it != m_procs.end(); ++it) {
    if (it->second->netflows.empty() && it->second->fileflows.empty() &&
        it->second->children.empty()) {
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
        it->second->children.empty()) {
      ProcessObj *proc = it->second;
      if (!proc->proc.containerId.is_null()) {
        m_containerCxt->derefContainer(proc->proc.containerId.get_string());
      }
      m_procs.erase(it);
      delete proc;
    }
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
  for (ProcessTable::iterator it = m_procs.begin(); it != m_procs.end(); ++it) {
    if (((!it->second->netflows.empty()) || (!it->second->fileflows.empty())) &&
        !it->second->written) {
      writeProcessAndAncestors(it->second);
    }
    for (NetworkFlowTable::iterator nfi = it->second->netflows.begin();
         nfi != it->second->netflows.end(); nfi++) {
      nfi->second->netflow.opFlags |= OP_TRUNCATE;
      nfi->second->netflow.endTs = utils::getSysdigTime(m_cxt);
      m_writer->writeNetFlow(&(nfi->second->netflow));
      delete nfi->second;
    }
    for (FileFlowTable::iterator ffi = it->second->fileflows.begin();
         ffi != it->second->fileflows.end(); ffi++) {
      ffi->second->fileflow.opFlags |= OP_TRUNCATE;
      ffi->second->fileflow.endTs = utils::getSysdigTime(m_cxt);
      m_fileCxt->exportFile(ffi->second->filekey);
      m_writer->writeFileFlow(&(ffi->second->fileflow));
      delete ffi->second;
    }
  }

  for (ProcessTable::iterator it = m_procs.begin(); it != m_procs.end(); ++it) {
    delete it->second;
  }
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
  m_procs.erase(&((*proc)->proc.oid));
  delete *proc;
  *proc = nullptr;
}

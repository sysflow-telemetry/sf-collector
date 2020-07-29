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

#ifndef _SF_PROC_
#define _SF_PROC_
#include "containercontext.h"
#include "datatypes.h"
#include "filecontext.h"
#include "logger.h"
#include "op_flags.h"
#include "sysflow.h"
#include "sysflowcontext.h"
#include "utils.h"
#include "api/sfinspector.h"

#define PROC_TABLE_SIZE 50000
#define PROC_DEL_EXPIRED 1.0

namespace process {
class ProcessContext {
private:
  context::SysFlowContext *m_cxt;
  writer::SysFlowWriter *m_writer;
  container::ContainerContext *m_containerCxt;
  ProcessTable m_procs;
  file::FileContext *m_fileCxt;
  OIDQueue m_delProcQue;
  ProcessFlowSet m_pfSet;
  time_t m_delProcTime;
  DEFINE_LOGGER();
  void writeProcessAndAncestors(ProcessObj *proc);
  void reupContainer(api::SysFlowProcess *pr, ProcessObj *proc);

public:
  ProcessContext(context::SysFlowContext *cxt,
                 container::ContainerContext *ccxt, file::FileContext *fileCxt,
                 writer::SysFlowWriter *writer);
  virtual ~ProcessContext();
  void updateProcess(Process *proc, api::SysFlowEvent *ev, SFObjectState state);
  ProcessObj *createProcess(api::SysFlowProcess *pr, api::SysFlowEvent *ev,
                            SFObjectState state);
  ProcessObj *getProcess(api::SysFlowEvent *ev, SFObjectState state,
                         bool &created);
  ProcessObj *getProcess(OID *oid);
  ProcessObj *getProcess(int64_t pid);
  void printAncestors(Process *proc);
  bool isAncestor(OID *oid, Process *proc);
  void clearProcesses();
  void clearAllProcesses();
  void deleteProcess(ProcessObj **proc);
  void markForDeletion(ProcessObj **proc);
  bool exportProcess(OID *oid);
  void printNetworkFlow(ProcessObj *proc);
  void printStats();
  int removeProcessFromSet(ProcessObj *proc, bool checkForErr);
  inline int getSize() { return m_procs.size(); }
  inline int getNumNetworkFlows() {
    int total = 0;
    for (ProcessTable::iterator it = m_procs.begin(); it != m_procs.end();
         it++) {
      total += it->second->netflows.size();
    }
    return total;
  }
  inline int getNumFileFlows() {
    int total = 0;
    for (ProcessTable::iterator it = m_procs.begin(); it != m_procs.end();
         it++) {
      total += it->second->fileflows.size();
    }
    return total;
  }

  inline void checkForDeletion() {
    time_t curTime = utils::getCurrentTime(m_cxt);
    if (difftime(curTime, m_delProcTime) <= PROC_DEL_EXPIRED) {
      return;
    }
    SF_DEBUG(m_logger, "Checking process queue for deletion. Queue Size: "
                           << m_delProcQue.size())
    for (auto it = m_delProcQue.begin(); it != m_delProcQue.end(); ++it) {
      if (difftime(curTime, (*it)->exportTime) >= PROC_DEL_EXPIRED) {
        SF_DEBUG(m_logger, "Proc expired: " << (*it)->oid.hpid)
        ProcessObj *p = getProcess(&((*it)->oid));
        if (p != nullptr) {
          SF_DEBUG(m_logger, "Deleting process: " << p->proc.oid.hpid)
          deleteProcess(&p);
        } else {
          SF_DEBUG(m_logger,
                   "Unable to find process in cache: " << (*it)->oid.hpid)
        }
        delete (*it);
        it = m_delProcQue.erase(it);
      } else {
        break;
      }
    }
    m_delProcTime = utils::getCurrentTime(m_cxt);
  }

  ProcessFlowSet *getPFSet();
};
} // namespace process
#endif

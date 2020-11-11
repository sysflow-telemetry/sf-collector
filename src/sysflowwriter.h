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

#ifndef __SF_WRITER_
#define __SF_WRITER_
#include "sysflow.h"
#include "sysflowcontext.h"
#include "utils.h"
#include "op_flags.h"

using sysflow::Container;
using sysflow::File;
using sysflow::FileEvent;
using sysflow::FileFlow;
using sysflow::NetworkFlow;
using sysflow::ProcessFlow;
using sysflow::Process;
using sysflow::ProcessEvent;
using sysflow::SysFlow;

namespace writer {
class SysFlowWriter {
protected:
  context::SysFlowContext *m_cxt;
  SysFlow m_flow;
  int m_numRecs{};
  void writeHeader();
  time_t m_start;
  int64_t m_version;
  virtual void write(SysFlow *flow) = 0;

public:
  SysFlowWriter(context::SysFlowContext *cxt, time_t start);
  virtual ~SysFlowWriter() {}
  inline int getNumRecs() { return m_numRecs; }
  inline void writeContainer(Container *container) {
    m_flow.rec.set_Container(*container);
    m_numRecs++;
    write(&m_flow);
  }
  inline void writeProcess(Process *proc) {
    m_flow.rec.set_Process(*proc);
    m_numRecs++;
    write(&m_flow);
  }
  inline void writeProcessEvent(ProcessEvent *pe) {
    m_flow.rec.set_ProcessEvent(*pe);
    m_numRecs++;
    write(&m_flow);
  }
  inline void writeNetFlow(NetworkFlow *nf) {
    if (nf->opFlags == 0 || nf->opFlags == OP_TRUNCATE) {
      return;
    }
    m_flow.rec.set_NetworkFlow(*nf);
    m_numRecs++;
    write(&m_flow);
  }
  inline void writeProcessFlow(ProcessFlow *pf) {
    if (pf->opFlags == 0 || pf->opFlags == OP_TRUNCATE) {
      return;
    }
    m_flow.rec.set_ProcessFlow(*pf);
    m_numRecs++;
    write(&m_flow);
  }
  inline void writeFileFlow(FileFlow *ff) {
    if (ff->opFlags == 0 || ff->opFlags == OP_TRUNCATE) {
      return;
    }
    m_flow.rec.set_FileFlow(*ff);
    m_numRecs++;
    write(&m_flow);
  }
  inline void writeFileEvent(FileEvent *fe) {
    m_flow.rec.set_FileEvent(*fe);
    m_numRecs++;
    write(&m_flow);
  }
  inline void writeFile(sysflow::File *f) {
    m_flow.rec.set_File(*f);
    m_numRecs++;
    write(&m_flow);
  }
  inline bool isExpired(time_t curTime) {
    if (m_start > 0) {
      double duration = getDuration(curTime);
      return (duration >= m_cxt->getFileDuration());
    }
    return false;
  }
  inline double getDuration(time_t curTime) {
    return difftime(curTime, m_start);
  }
  virtual int initialize() = 0;
  virtual void reset(time_t curTime) = 0;
};
} // namespace writer
#endif

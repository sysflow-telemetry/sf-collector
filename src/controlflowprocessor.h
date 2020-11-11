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

#ifndef _SF_CONT_FLOW_
#define _SF_CONT_FLOW_
#include "processeventprocessor.h"
#include "logger.h"
#include "op_flags.h"
#include "sysflowcontext.h"
#include "sysflowwriter.h"
#include "utils.h"
#include <sinsp.h>

namespace controlflow {
class ControlFlowProcessor {
private:
  processevent::ProcessEventProcessor *m_procEvtPrcr;
  context::SysFlowContext *m_cxt;
  process::ProcessContext *m_processCxt;
  writer::SysFlowWriter *m_writer;
  ProcessFlowSet* m_pfSet;
  time_t m_lastCheck;
  DEFINE_LOGGER();
  void updateProcFlow(ProcessFlowObj *pf, OpFlags flag, sinsp_evt *ev);
  void populateProcFlow(ProcessFlowObj *pf, OpFlags flag, sinsp_evt *ev, ProcessObj *proc); 
  void processNewFlow(sinsp_evt *ev, ProcessObj *proc, OpFlags flag);
  void processFlow(sinsp_evt *ev, OpFlags flag);
  void removeAndWriteProcessFlow(ProcessObj *proc);


public:
  inline int getSize() { return m_pfSet->size(); }
  int handleProcEvent(sinsp_evt *ev, OpFlags flag);
  ControlFlowProcessor(context::SysFlowContext *cxt, writer::SysFlowWriter *writer,
                    process::ProcessContext *processCxt,
		    dataflow::DataFlowProcessor *dfPrcr);
  virtual ~ControlFlowProcessor();
  int checkForExpiredRecords();
  void printFlowStats();
  void exportProcessFlow(ProcessFlowObj *pfo);
  void setUID(sinsp_evt *ev);
};
} // namespace dataflow

#endif

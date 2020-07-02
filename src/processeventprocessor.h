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

#ifndef _SF_PROC_EVT_
#define _SF_PROC_EVT_
#include "dataflowprocessor.h"
#include "logger.h"
#include "processcontext.h"
#include "sysflow.h"
#include "sysflowwriter.h"
#include <string>
#include "api/sfinspector.h"

namespace processevent {
class ProcessEventProcessor {
public:
  ProcessEventProcessor(writer::SysFlowWriter *writer,
                        process::ProcessContext *pc,
                        dataflow::DataFlowProcessor *dfPrcr);
  virtual ~ProcessEventProcessor();
  void writeCloneEvent(api::SysFlowEvent *ev);
  void writeExitEvent(api::SysFlowEvent *ev);
  void writeExecEvent(api::SysFlowEvent *ev);
  void writeSetUIDEvent(api::SysFlowEvent *ev);
  void setUID(api::SysFlowEvent *ev);

private:
  writer::SysFlowWriter *m_writer;
  process::ProcessContext *m_processCxt;
  dataflow::DataFlowProcessor *m_dfPrcr;
  sysflow::ProcessEvent m_procEvt;
  std::string m_uid;
  DEFINE_LOGGER();
};
} // namespace processevent
#endif

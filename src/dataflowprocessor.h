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

#ifndef _SF_DATA_FLOW_
#define _SF_DATA_FLOW_
#include "fileeventprocessor.h"
#include "fileflowprocessor.h"
#include "logger.h"
#include "networkflowprocessor.h"
#include "op_flags.h"
#include "sysflowcontext.h"
#include "sysflowwriter.h"
#include <sinsp.h>

namespace dataflow {
class DataFlowProcessor {
private:
  networkflow::NetworkFlowProcessor *m_netflowPrcr;
  fileflow::FileFlowProcessor *m_fileflowPrcr;
  fileevent::FileEventProcessor *m_fileevtPrcr;
  context::SysFlowContext *m_cxt;
  DataFlowSet m_dfSet;
  time_t m_lastCheck;
  DEFINE_LOGGER();

public:
  inline int getSize() { return m_netflowPrcr->getSize(); }
  int handleDataEvent(sinsp_evt *ev, OpFlags flag);
  DataFlowProcessor(context::SysFlowContext *cxt, writer::SysFlowWriter *writer,
                    process::ProcessContext *processCxt,
                    file::FileContext *fileCxt);
  virtual ~DataFlowProcessor();
  int checkForExpiredRecords();
  int removeAndWriteDFFromProc(ProcessObj *proc, int64_t tid);
};
} // namespace dataflow

#endif

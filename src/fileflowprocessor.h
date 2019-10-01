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

#ifndef _SF_FILE_FLOW_
#define _SF_FILE_FLOW_
#include "datatypes.h"
#include "file_types.h"
#include "filecontext.h"
#include "logger.h"
#include "op_flags.h"
#include "processcontext.h"
#include "sysflow.h"
#include "sysflowcontext.h"
#include "sysflowwriter.h"
#include <ctime>
#include <sinsp.h>

namespace fileflow {
class FileFlowProcessor {
private:
  context::SysFlowContext *m_cxt;
  process::ProcessContext *m_processCxt;
  writer::SysFlowWriter *m_writer;
  DataFlowSet *m_dfSet;
  file::FileContext *m_fileCxt;
  void populateFileFlow(FileFlowObj *ff, OpFlags flag, sinsp_evt *ev,
                        ProcessObj *proc, FileObj *file, string flowkey);
  void updateFileFlow(FileFlowObj *ff, OpFlags flag, sinsp_evt *ev);
  void processExistingFlow(sinsp_evt *ev, ProcessObj *proc, FileObj *file,
                           OpFlags flag, string flowkey, FileFlowObj *ff);
  void processNewFlow(sinsp_evt *ev, ProcessObj *proc, FileObj *file,
                      OpFlags flag, const string &flowkey);
  void removeAndWriteFileFlow(ProcessObj *proc, FileObj *file, FileFlowObj **nf,
                              string flowkey);
  void removeFileFlow(ProcessObj *proc, FileObj *file, FileFlowObj **ff,
                      const string &flowkey);
  int removeFileFlowFromSet(FileFlowObj **ffo, bool deleteFileFlow);
  void removeAndWriteRelatedFlows(ProcessObj *proc, FileFlowObj *ffo,
                                  uint64_t endTs);
  DEFINE_LOGGER();

public:
  FileFlowProcessor(context::SysFlowContext *cxt, writer::SysFlowWriter *writer,
                    process::ProcessContext *procCxt, DataFlowSet *dfSet,
                    file::FileContext *fileCxt);
  virtual ~FileFlowProcessor();
  int handleFileFlowEvent(sinsp_evt *ev, OpFlags flag);
  inline int getSize() { return m_processCxt->getNumFileFlows(); }
  int removeAndWriteFFFromProc(ProcessObj *proc, int64_t tid);
  void removeFileFlow(DataFlowObj *dfo);
  void exportFileFlow(DataFlowObj *dfo, time_t now);
};
} // namespace fileflow
#endif

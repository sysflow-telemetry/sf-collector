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

#ifndef _SF_FILE_EVT
#define _SF_FILE_EVT
#include "file_types.h"
#include "filecontext.h"
#include "logger.h"
#include "processcontext.h"
#include "syscall_defs.h"
#include "sysflow.h"
#include "sysflowwriter.h"
#include "utils.h"
#include <sinsp.h>
using namespace sysflow;
namespace fileevent {

class FileEventProcessor {
private:
  process::ProcessContext *m_processCxt;
  SysFlowWriter *m_writer;
  file::FileContext *m_fileCxt;
  FileEvent m_fileEvt;
  int writeFileEvent(sinsp_evt *ev, OpFlags flag);
  int writeLinkEvent(sinsp_evt *ev, OpFlags flag);
  DEFINE_LOGGER();

public:
  FileEventProcessor(SysFlowWriter *writer, process::ProcessContext *procCxt,
                     file::FileContext *fileCxt);
  virtual ~FileEventProcessor();
  int handleFileFlowEvent(sinsp_evt *ev, OpFlags flag);
};
} // namespace fileevent
#endif

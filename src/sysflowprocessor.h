/** Copyright (C) 2019 IBM Corporation.
 *
 * Authors:
 * Teryl Taylor <terylt@ibm.com>
 * Frederico Araujo <frederico.araujo@ibm.com>
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

#ifndef __SF_PROCESSOR_
#define __SF_PROCESSOR_
#include "containercontext.h"
#include "dataflowprocessor.h"
#include "filecontext.h"
#include "logger.h"
#include "processcontext.h"
#include "controlflowprocessor.h"
#include "sffilewriter.h"
#include "sfsockwriter.h"
#include "syscall_defs.h"
#include "sysflowcontext.h"
#include <ctime>
#include <string>
#include <system_error>
#include "api/sfinspector.h"

namespace sysflowprocessor {
class SysFlowProcessor {

private:
  DEFINE_LOGGER();
  bool m_exit;
  context::SysFlowContext *m_cxt;
  writer::SysFlowWriter *m_writer;
  container::ContainerContext *m_containerCxt;
  file::FileContext *m_fileCxt;
  process::ProcessContext *m_processCxt;
  controlflow::ControlFlowProcessor* m_ctrlPrcr;
  dataflow::DataFlowProcessor *m_dfPrcr;
  void clearTables();
  bool checkAndRotateFile();
  int checkForExpiredRecords();
  time_t m_statsTime;
public:
  explicit SysFlowProcessor(context::SysFlowContext *cxt);
  virtual ~SysFlowProcessor();
  inline void exit() { SF_INFO(m_logger, "EXIT Called"); m_exit = true; }
  int run();
};
} // namespace sysflowprocessor

#endif

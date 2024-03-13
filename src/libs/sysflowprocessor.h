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
#include "controlflowprocessor.h"
#include "dataflowprocessor.h"
#include "filecontext.h"
#include "k8scontext.h"
#include "k8seventprocessor.h"
#include "logger.h"
#include "processcontext.h"
#include "sffilewriter.h"
#include "sfmultiwriter.h"
#include "sfsockwriter.h"
#include "syscall_defs.h"
#include "sysflowcontext.h"
#include <ctime>
#include <stdlib.h>
#include <string>
#include <thread>

namespace sysflowprocessor {
class SysFlowProcessor {
public:
  explicit SysFlowProcessor(context::SysFlowContext *cxt,
                            writer::SysFlowWriter *writer = nullptr);
  virtual ~SysFlowProcessor();
  inline void exit() { m_exit = true; }
  int run();
  sysflow::Container *getContainer(const std::string &containerId);
  sysflow::Process *getProcess(sysflow::OID &oid);

private:
  DEFINE_LOGGER();
  bool m_exit;
  writer::SysFlowWriter *m_writer;
  context::SysFlowContext *m_cxt;
  container::ContainerContext *m_containerCxt;
  file::FileContext *m_fileCxt;
  process::ProcessContext *m_processCxt;
  controlflow::ControlFlowProcessor *m_ctrlPrcr;
  dataflow::DataFlowProcessor *m_dfPrcr;
  sfk8s::K8sContext *m_k8sCxt;
  k8sevent::K8sEventProcessor *m_k8sPrcr;
  time_t m_statsTime;
  void clearTables();
  int checkForExpiredRecords();
  bool checkAndRotateFile();
  void printStats();
};
} // namespace sysflowprocessor

#endif

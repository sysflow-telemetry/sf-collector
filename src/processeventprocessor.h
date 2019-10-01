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
#include <sinsp.h>
#include "sysflow.h"
#include "sysflowwriter.h"
#include "processcontext.h"
#include "dataflowprocessor.h"
#include "utils.h"
#include <string>
#include "logger.h"
#define SF_UID "uid"
using namespace sysflow;
namespace processevent {
    class ProcessEventProcessor {
       public:
          ProcessEventProcessor(SysFlowWriter* writer, process::ProcessContext* pc, dataflow::DataFlowProcessor* dfPrcr);
          virtual ~ProcessEventProcessor();
          void writeCloneEvent(sinsp_evt* ev);
          void writeExitEvent(sinsp_evt* ev);
          void writeExecEvent(sinsp_evt* ev);
          void writeSetUIDEvent(sinsp_evt* ev);
          void setUID(sinsp_evt* ev);
       private:
          SysFlowWriter* m_writer;
          process::ProcessContext* m_processCxt;
          dataflow::DataFlowProcessor* m_dfPrcr;
          ProcessEvent m_procEvt;
          std::string m_uid;
	  DEFINE_LOGGER();
   };
   } // namespace processevent
#endif

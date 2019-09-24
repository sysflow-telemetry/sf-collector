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

#ifndef  __SF_PROCESSOR_
#define  __SF_PROCESSOR_
#include <time.h>
#include <string>
#include "syscall_defs.h"
#include "sysflowcontext.h"
#include "processcontext.h"
#include "filecontext.h"
#include "containercontext.h"
#include "processeventprocessor.h"
#include "dataflowprocessor.h"
#include "logger.h" 

namespace sysflowprocessor {
     class SysFlowProcessor {
        public:
            SysFlowProcessor(SysFlowContext* cxt);
            virtual ~SysFlowProcessor();
            inline void exit() {
                m_exit = true;
            }
            int run();
        private:
	    DEFINE_LOGGER();
            bool m_exit;
            SysFlowContext* m_cxt;
            SysFlowWriter* m_writer;
            container::ContainerContext* m_containerCxt;
            file::FileContext* m_fileCxt;           
            process::ProcessContext* m_processCxt;
            processevent::ProcessEventProcessor* m_procEvtPrcr;
            dataflow::DataFlowProcessor* m_dfPrcr;
            void clearTables();
            bool checkAndRotateFile();
            //static log4cxx::LoggerPtr m_logger;
     };
}

#endif 

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

#ifndef _SF_PROC_
#define _SF_PROC_
#include <sinsp.h>
#include "sysflow.h"
#include "sysflowcontext.h"
#include "datatypes.h"
#include "containercontext.h"
#include "filecontext.h"
#include "utils.h"
#include "op_flags.h"
#include "logger.h"

#define PROC_TABLE_SIZE 50000
using namespace sysflow;
namespace process {
    class ProcessContext {
        private:
            SysFlowContext* m_cxt;
            SysFlowWriter* m_writer;
            container::ContainerContext* m_containerCxt;
            ProcessTable m_procs;
            file::FileContext* m_fileCxt;
	    DEFINE_LOGGER();
            void writeProcessAndAncestors(ProcessObj* proc); 
            void reupContainer(sinsp_evt* ev, ProcessObj* proc);
        public:
            ProcessContext(SysFlowContext* cxt, container::ContainerContext* ccxt, file::FileContext* fileCxt, SysFlowWriter* writer);
            virtual ~ProcessContext();
            void updateProcess(Process* proc, sinsp_evt* ev, SFObjectState state);
            ProcessObj* createProcess(sinsp_threadinfo* mainthread, sinsp_evt* ev, SFObjectState state);
            ProcessObj* getProcess(sinsp_evt* ev, SFObjectState state, bool& created);
            ProcessObj* getProcess(OID* oid);
            void printAncestors(Process* proc);
            bool isAncestor(OID* oid, Process* proc);
            void clearProcesses();
            void clearAllProcesses();
            void deleteProcess(ProcessObj** proc);
            bool exportProcess(OID* oid);
            void printNetworkFlow(ProcessObj* proc);
            inline int getSize() {
                return m_procs.size();
            }
            inline int getNumNetworkFlows() {
                int total = 0;
                for(ProcessTable::iterator it = m_procs.begin(); it != m_procs.end(); it++) {
                     total+= it->second->netflows.size();
                 }
                return total;
            }
            inline int getNumFileFlows() {
                int total = 0;
                for(ProcessTable::iterator it = m_procs.begin(); it != m_procs.end(); it++) {
                     total+= it->second->fileflows.size();
                 }
                return total;
            }


    };
    } // namespace process
#endif

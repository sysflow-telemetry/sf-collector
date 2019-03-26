#ifndef _SF_PROC_
#define _SF_PROC_
#include <sinsp.h>
#include "sysflow/sysflow.hh"
#include "sysflowcontext.h"
#include "datatypes.h"
#include "containercontext.h"
#include "container.h"
#include "utils.h"

#define PROC_TABLE_SIZE 50000
using namespace sysflow;
namespace process {
    class ProcessContext {
        private:
            SysFlowContext* m_cxt;
            SysFlowWriter* m_writer;
            container::ContainerContext* m_containerCxt;
            ProcessTable m_procs;
            OID m_empkey;
            OID m_delkey;
        public:
            ProcessContext(SysFlowContext* cxt, container::ContainerContext* ccxt, SysFlowWriter* writer);
            virtual ~ProcessContext();
            void updateProcess(Process* proc, sinsp_evt* ev, ActionType actType);
            Process* createProcess(sinsp_threadinfo* mainthread, sinsp_evt* ev, ActionType actType);
            Process* getProcess(sinsp_evt* ev, ActionType actType, bool& created);
            Process* getProcess(OID* oid);
            void printAncestors(Process* proc);
            bool isAncestor(OID* oid, Process* proc);
            void clearProcesses();
            void deleteProcess(Process** proc);
            inline int getSize() {
                return m_procs.size();
            }
    };
}
#endif

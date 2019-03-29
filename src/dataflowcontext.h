#ifndef _SF_DATA_FLOW_
#define _SF_DATA_FLOW_
#include <sinsp.h>
#include "sysflowcontext.h"
#include "op_flags.h"
#include "networkflowcontext.h"
#include "sysflowwriter.h"

namespace dataflow {
    class DataFlowContext {
        private:
           networkflow::NetworkFlowContext* m_netflowCxt;
           SysFlowContext*   m_cxt;
           DataFlowSet m_dfSet;
           time_t m_lastCheck;
        public:
           inline int getSize() {
               return m_netflowCxt->getSize();
           }
           int handleDataEvent(sinsp_evt* ev, OpFlags flag);
           DataFlowContext(SysFlowContext* cxt, SysFlowWriter* writer, process::ProcessContext* processCxt);
           virtual ~DataFlowContext();
           void clearTables();
           int checkForExpiredRecords();
           int removeAndWriteDFFromProc(ProcessObj* proc);
    };
}

#endif

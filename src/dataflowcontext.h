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
        public:
           inline int getSize() {
               return m_netflowCxt->getSize();
           }
           int handleDataEvent(sinsp_evt* ev, NFOpFlags flag);
           DataFlowContext(SysFlowContext* cxt, SysFlowWriter* writer, process::ProcessContext* processCxt);
           virtual ~DataFlowContext();
           void clearTables();
           int checkForExpiredRecords();
    };
}

#endif

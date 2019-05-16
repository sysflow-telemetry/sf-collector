#ifndef _SF_DATA_FLOW_
#define _SF_DATA_FLOW_
#include <sinsp.h>
#include "sysflowcontext.h"
#include "op_flags.h"
#include "networkflowprocessor.h"
#include "fileflowprocessor.h"
#include "fileeventprocessor.h"
#include "sysflowwriter.h"
#include "logger.h" 

namespace dataflow {
    class DataFlowProcessor {
        private:
            networkflow::NetworkFlowProcessor* m_netflowPrcr;
            fileflow::FileFlowProcessor* m_fileflowPrcr;
            fileevent::FileEventProcessor* m_fileevtPrcr;
            SysFlowContext*   m_cxt;
            DataFlowSet m_dfSet;
            time_t m_lastCheck;
            //static log4cxx::LoggerPtr m_logger;
        public:
            inline int getSize() {
                return m_netflowPrcr->getSize();
            }
            int handleDataEvent(sinsp_evt* ev, OpFlags flag);
            DataFlowProcessor(SysFlowContext* cxt, SysFlowWriter* writer, process::ProcessContext* processCxt, file::FileContext* fileCxt);
            virtual ~DataFlowProcessor();
            int checkForExpiredRecords();
            int removeAndWriteDFFromProc(ProcessObj* proc, int64_t tid);
    };
}

#endif

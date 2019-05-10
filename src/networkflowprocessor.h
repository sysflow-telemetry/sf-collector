#ifndef _SF_NET_FLOW_
#define _SF_NET_FLOW_
#include <ctime>
#include "datatypes.h"
#include <sinsp.h>
#include "op_flags.h"
#include "sysflowcontext.h"
#include "sysflowwriter.h"
#include "processcontext.h"
#include "sysflow.h"
#include "logger.h"
using namespace sysflow;
namespace networkflow {


    class NetworkFlowProcessor {
        private:
            SysFlowContext* m_cxt;
            process::ProcessContext* m_processCxt;
            SysFlowWriter* m_writer;
            DataFlowSet* m_dfSet;
            static log4cxx::LoggerPtr m_logger;
            void canonicalizeKey(sinsp_fdinfo_t* fdinfo, NFKey* key,  uint64_t tid);
            void canonicalizeKey(NetFlowObj* nf, NFKey* key);
            void populateNetFlow(NetFlowObj* nf, OpFlags flag, sinsp_evt* ev, ProcessObj* proc);
            void updateNetFlow(NetFlowObj* nf, OpFlags flag, sinsp_evt* ev);
            void processExistingFlow(sinsp_evt* ev, ProcessObj* proc, OpFlags flag, NFKey key, NetFlowObj* nf);
            void processNewFlow(sinsp_evt* ev, ProcessObj* proc, OpFlags flag, NFKey key) ;
            void removeAndWriteNetworkFlow(ProcessObj* proc, NetFlowObj** nf, NFKey* key);
            void removeNetworkFlow(ProcessObj* proc, NetFlowObj** nf, NFKey* key);
         
            int32_t getProtocol(scap_l4_proto proto);
            int removeNetworkFlowFromSet(NetFlowObj** nfo, bool deleteNetFlow);
        public:
            NetworkFlowProcessor(SysFlowContext* cxt, SysFlowWriter* writer, process::ProcessContext* procCxt, DataFlowSet* dfSet);
            virtual ~NetworkFlowProcessor();
            int handleNetFlowEvent(sinsp_evt* ev, OpFlags flag);
            inline int getSize() {
                 return m_processCxt->getNumNetworkFlows();
            }
            int removeAndWriteNFFromProc(ProcessObj* proc, int64_t tid);
            void removeNetworkFlow(DataFlowObj* dfo);
            void exportNetworkFlow(DataFlowObj* dfo, time_t now);
         
    };
}
#endif

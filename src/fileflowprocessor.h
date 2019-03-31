#ifndef _SF_FIlE_FLOW_
#define _SF_FILE_FLOW_
#include <ctime>
#include "datatypes.h"
#include <sinsp.h>
#include "op_flags.h"
#include "sysflowcontext.h"
#include "sysflowwriter.h"
#include "processcontext.h"
#include "sysflow/sysflow.hh"
using namespace sysflow;
namespace fileflow {


    class FileFlowProcessor {
        private:
            SysFlowContext* m_cxt;
            process::ProcessContext* m_processCxt;
            SysFlowWriter* m_writer;
            DataFlowSet* m_dfSet;
            //NetworkFlowTable m_netflows;
            //OIDNetworkTable m_oidnfTable;
            //OID m_oiddelkey;
            //OID m_oidemptykey;
            void canonicalizeKey(sinsp_fdinfo_t* fdinfo, NFKey* key);
            void canonicalizeKey(NetFlowObj* nf, NFKey* key);
            void populateNetFlow(NetFlowObj* nf, OpFlags flag, sinsp_evt* ev, ProcessObj* proc);
            void updateNetFlow(NetFlowObj* nf, OpFlags flag, sinsp_evt* ev);
            void processExistingFlow(sinsp_evt* ev, ProcessObj* proc, OpFlags flag, NFKey key, NetFlowObj* nf);
            void processNewFlow(sinsp_evt* ev, ProcessObj* proc, OpFlags flag, NFKey key) ;
            void removeAndWriteNetworkFlow(ProcessObj* proc, NetFlowObj** nf, NFKey* key);
            void removeNetworkFlow(ProcessObj* proc, NetFlowObj** nf, NFKey* key);
         
            int removeNetworkFlowFromSet(NetFlowObj** nfo, bool deleteNetFlow);
        public:
            FileFlowProcessor(SysFlowContext* cxt, SysFlowWriter* writer, process::ProcessContext* procCxt, DataFlowSet* dfSet);
            virtual ~FileFlowProcessor();
            int handleFileFlowEvent(sinsp_evt* ev, OpFlags flag);
            inline int getSize() {
                 return m_processCxt->getNumNetworkFlows();
            }
            int removeAndWriteNFFromProc(ProcessObj* proc);
            void removeNetworkFlow(DataFlowObj* dfo);
            void exportNetworkFlow(DataFlowObj* dfo, time_t now);
         
    };
}
#endif

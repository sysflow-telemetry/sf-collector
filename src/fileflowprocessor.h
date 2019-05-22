#ifndef _SF_FILE_FLOW_
#define _SF_FILE_FLOW_
#include <ctime>
#include "datatypes.h"
#include <sinsp.h>
#include "op_flags.h"
#include "sysflowcontext.h"
#include "sysflowwriter.h"
#include "processcontext.h"
#include "filecontext.h"
#include "sysflow.h"
#include "file_types.h"
#include "logger.h"
using namespace sysflow;
namespace fileflow {


    class FileFlowProcessor {
        private:
            SysFlowContext* m_cxt;
            process::ProcessContext* m_processCxt;
            SysFlowWriter* m_writer;
            DataFlowSet* m_dfSet;
            file::FileContext* m_fileCxt;
            void populateFileFlow(sinsp_fdinfo_t * fdinfo, FileFlowObj* ff, OpFlags flag, sinsp_evt* ev, ProcessObj* proc, FileObj* file, string flowkey);
            void updateFileFlow(FileFlowObj* ff, OpFlags flag, sinsp_evt* ev);
            void processExistingFlow(sinsp_evt* ev, ProcessObj* proc, FileObj* file, OpFlags flag, string flowkey, FileFlowObj* ff);
            void processNewFlow(sinsp_evt* ev, sinsp_fdinfo_t * fdinfo, ProcessObj* proc, FileObj* file,  OpFlags flag, string flowkey) ;
            void removeAndWriteFileFlow(ProcessObj* proc, FileObj* file,  FileFlowObj** nf, string flowkey);
            void removeFileFlow(ProcessObj* proc, FileObj* file, FileFlowObj** ff, string flowkey);
            int removeFileFlowFromSet(FileFlowObj** ffo, bool deleteFileFlow);
	    void removeAndWriteRelatedFlows(ProcessObj* proc, FileFlowObj* ffo, uint64_t endTs);
	    DEFINE_LOGGER();
        public:
            FileFlowProcessor(SysFlowContext* cxt, SysFlowWriter* writer, process::ProcessContext* procCxt, DataFlowSet* dfSet, file::FileContext* fileCxt);
            virtual ~FileFlowProcessor();
            int handleFileFlowEvent(sinsp_evt* ev, OpFlags flag);
            inline int getSize() {
                 return m_processCxt->getNumFileFlows();
            }
            int removeAndWriteFFFromProc(ProcessObj* proc, int64_t tid);
            void removeFileFlow(DataFlowObj* dfo);
            void exportFileFlow(DataFlowObj* dfo, time_t now);
         
    };
}
#endif

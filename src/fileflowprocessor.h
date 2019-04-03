#ifndef _SF_FIlE_FLOW_
#define _SF_FILE_FLOW_
#include <ctime>
#include "datatypes.h"
#include <sinsp.h>
#include "op_flags.h"
#include "sysflowcontext.h"
#include "sysflowwriter.h"
#include "processcontext.h"
#include "filecontext.h"
#include "sysflow/sysflow.hh"
#include "file_types.h"
using namespace sysflow;
namespace fileflow {


    class FileFlowProcessor {
        private:
            SysFlowContext* m_cxt;
            process::ProcessContext* m_processCxt;
            SysFlowWriter* m_writer;
            DataFlowSet* m_dfSet;
            file::FileContext* m_fileCxt;
            void populateFileFlow(FileFlowObj* ff, OpFlags flag, sinsp_evt* ev, ProcessObj* proc, FileObj* file);
            void updateFileFlow(FileFlowObj* ff, OpFlags flag, sinsp_evt* ev);
            void processExistingFlow(sinsp_evt* ev, ProcessObj* proc, FileObj* file, OpFlags flag, string key, FileFlowObj* ff);
            void processNewFlow(sinsp_evt* ev, ProcessObj* proc, FileObj* file,  OpFlags flag, string key) ;
            void removeAndWriteFileFlow(ProcessObj* proc, FileObj* file,  FileFlowObj** nf, string key);
            void removeFileFlow(ProcessObj* proc, FileObj* file, FileFlowObj** ff, string key);
         
            int removeFileFlowFromSet(FileFlowObj** ffo, bool deleteFileFlow);
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

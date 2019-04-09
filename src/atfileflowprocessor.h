#ifndef _SF_ATOMIC_FILE_FLOW
#define _SF_ATOMIC_FILE_FLOW
#include <sinsp.h>
#include "sysflowwriter.h"
#include "processcontext.h"
#include "filecontext.h"
#include "sysflow/sysflow.hh"
#include "utils.h"
using namespace sysflow;
namespace atomicflow {


    class AtomicFileFlowProcessor {
        private:
            process::ProcessContext* m_processCxt;
            SysFlowWriter* m_writer;
            file::FileContext* m_fileCxt;
            AtomicFileFlow m_atFileFlow;
            int writeFileEvent(sinsp_evt* ev, OpFlags flag);
            int writeLinkEvent(sinsp_evt* ev, OpFlags flag);
        public:
            AtomicFileFlowProcessor(SysFlowWriter* writer, process::ProcessContext* procCxt, file::FileContext* fileCxt);
            virtual ~AtomicFileFlowProcessor();
            int handleFileFlowEvent(sinsp_evt* ev, OpFlags flag);
                 

    };

}
#endif

#ifndef _SF_FILE_EVT
#define _SF_FILE_EVT
#include <sinsp.h>
#include "sysflowwriter.h"
#include "processcontext.h"
#include "filecontext.h"
#include "sysflow.h"
#include "utils.h"
#include "syscall_defs.h"
#include "file_types.h"
#include "logger.h"
using namespace sysflow;
namespace fileevent {


    class FileEventProcessor {
        private:
            process::ProcessContext* m_processCxt;
            SysFlowWriter* m_writer;
            file::FileContext* m_fileCxt;
            FileEvent m_fileEvt;
            int writeFileEvent(sinsp_evt* ev, OpFlags flag);
            int writeLinkEvent(sinsp_evt* ev, OpFlags flag);
            DEFINE_LOGGER();
        public:
            FileEventProcessor(SysFlowWriter* writer, process::ProcessContext* procCxt, file::FileContext* fileCxt);
            virtual ~FileEventProcessor();
            int handleFileFlowEvent(sinsp_evt* ev, OpFlags flag);
                 

    };

}
#endif

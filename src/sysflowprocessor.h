#ifndef  __SF_PROCESSOR_
#define  __SF_PROCESSOR_
#include <time.h>
#include "syscall_defs.h"
#include "sysflowcontext.h"
#include "processcontext.h"
#include "filecontext.h"
#include "containercontext.h"
#include "processeventprocessor.h"
#include "dataflowprocessor.h"
#include <string> 

namespace sysflowprocessor {
     class SysFlowProcessor {
        public:
            SysFlowProcessor(SysFlowContext* cxt);
            virtual ~SysFlowProcessor();
            inline void exit() {
                m_exit = true;
            }
            int run();
        private:
            bool m_exit;
            SysFlowContext* m_cxt;
            SysFlowWriter* m_writer;
            container::ContainerContext* m_containerCxt;
            file::FileContext* m_fileCxt;           
            process::ProcessContext* m_processCxt;
            processevent::ProcessEventProcessor* m_procEvtPrcr;
            dataflow::DataFlowProcessor* m_dfPrcr;
            void clearTables();
            bool checkAndRotateFile();
     };
}

#endif 

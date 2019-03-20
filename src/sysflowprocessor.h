#ifndef  __SF_PROCESSOR_
#define  __SF_PROCESSOR_
#include <time.h>
#include "syscall_defs.h"
#include "sysflowcontext.h"
#include "processcontext.h"
#include "containercontext.h"
#include "processflowcontext.h"
#include "dataflowcontext.h"
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
            process::ProcessContext* m_processCxt;
            processflow::ProcessFlowContext* m_procFlowCxt;
            dataflow::DataFlowContext* m_dfCxt;
            void clearTables();
            bool checkAndRotateFile();
     };
}

#endif 

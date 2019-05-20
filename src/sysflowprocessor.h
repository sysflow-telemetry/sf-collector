#ifndef  __SF_PROCESSOR_
#define  __SF_PROCESSOR_
#include <time.h>
#include <string>
#include "syscall_defs.h"
#include "sysflowcontext.h"
#include "processcontext.h"
#include "filecontext.h"
#include "containercontext.h"
#include "processeventprocessor.h"
#include "dataflowprocessor.h"
#include "logger.h" 

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
	    DEFINE_LOGGER();
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
            //static log4cxx::LoggerPtr m_logger;
     };
}

#endif 

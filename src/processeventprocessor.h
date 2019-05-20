#ifndef _SF_PROC_EVT_
#define _SF_PROC_EVT_
#include <sinsp.h>
#include "sysflow.h"
#include "sysflowwriter.h"
#include "processcontext.h"
#include "dataflowprocessor.h"
#include "utils.h"
#include <string>
#include "logger.h"
#define SF_UID "uid"
using namespace sysflow;
namespace processevent {
    class ProcessEventProcessor {
       public:
          ProcessEventProcessor(SysFlowWriter* writer, process::ProcessContext* pc, dataflow::DataFlowProcessor* dfPrcr);
          virtual ~ProcessEventProcessor();
          void writeCloneEvent(sinsp_evt* ev);
          void writeExitEvent(sinsp_evt* ev);
          void writeExecEvent(sinsp_evt* ev);
          void writeSetUIDEvent(sinsp_evt* ev);
          void setUID(sinsp_evt* ev);
       private:
          SysFlowWriter* m_writer;
          process::ProcessContext* m_processCxt;
          dataflow::DataFlowProcessor* m_dfPrcr;
          ProcessEvent m_procEvt;
          std::string m_uid;
	  DEFINE_LOGGER();
   };
}
#endif

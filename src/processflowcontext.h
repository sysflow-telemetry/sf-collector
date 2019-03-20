#ifndef _SF_PROC_FLOW_
#define _SF_PROC_FLOW_
#include <sinsp.h>
#include "sysflow/sysflow.hh"
#include "sysflowwriter.h"
#include "processcontext.h"
#include "utils.h"

using namespace sysflow;
namespace processflow {
    class ProcessFlowContext {
       public:
          ProcessFlowContext(SysFlowWriter* writer, process::ProcessContext* pc);
          virtual ~ProcessFlowContext();
          void writeCloneEvent(sinsp_evt* ev);
          void writeExitEvent(sinsp_evt* ev);
          void writeExecEvent(sinsp_evt* ev);
       private:
          SysFlowWriter* m_writer;
          process::ProcessContext* m_processCxt;
          ProcessFlow m_procFlow;
   };
}
#endif

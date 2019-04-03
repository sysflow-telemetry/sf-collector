#include "processflowprocessor.h"

using namespace processflow;
ProcessFlowProcessor::ProcessFlowProcessor(SysFlowWriter* writer, process::ProcessContext* pc, dataflow::DataFlowProcessor* dfPrcr) {
    m_processCxt = pc;
    m_writer = writer;
    m_dfPrcr = dfPrcr;
}

ProcessFlowProcessor::~ProcessFlowProcessor() {

}

void ProcessFlowProcessor::writeCloneEvent(sinsp_evt* ev) {
    sinsp_threadinfo* ti = ev->get_thread_info();
    bool created = false;
    ProcessObj* proc = m_processCxt->getProcess(ev, SFObjectState::CREATED, created);
    m_procFlow.opFlags =  OP_CLONE;
    m_procFlow.ts = ev->get_ts();
    m_procFlow.procOID.hpid = proc->proc.oid.hpid;
    m_procFlow.procOID.createTS = proc->proc.oid.createTS;
    m_procFlow.tid = ti->m_tid;
    m_procFlow.ret = utils::getSyscallResult(ev);
    m_writer->writeProcessFlow(&m_procFlow);
}

void ProcessFlowProcessor::writeExitEvent(sinsp_evt* ev) {
    sinsp_threadinfo* ti = ev->get_thread_info();
    bool created = false;
    ProcessObj* proc = m_processCxt->getProcess(ev, SFObjectState::REUP, created);
    m_procFlow.opFlags =  OP_EXIT;
    m_procFlow.ts = ev->get_ts();
    m_procFlow.procOID.hpid = proc->proc.oid.hpid;
    m_procFlow.procOID.createTS = proc->proc.oid.createTS;
    m_procFlow.tid = ti->m_tid;
    m_procFlow.ret = utils::getSyscallResult(ev);
    int64_t tid = -1;
    if(!ti->is_main_thread()) {
         tid = ti->m_tid;
    }
    m_dfPrcr->removeAndWriteDFFromProc(proc, tid);
    m_writer->writeProcessFlow(&m_procFlow);
    // delete the process from the proc table after an exit
    if(ti->is_main_thread()) {
        m_processCxt->deleteProcess(&proc);
    }
}

void ProcessFlowProcessor::writeExecEvent(sinsp_evt* ev) {
    sinsp_threadinfo* ti = ev->get_thread_info();
    bool created = false;
    ProcessObj* proc = m_processCxt->getProcess(ev, SFObjectState::CREATED, created);
   /* if(!created) {
      cout << "Exec on an existing process!" << endl;
    }*/

    //If Clones are filtered out (or a process like bash is filtered out) then we will only see
    // the EXEC of this process, and the getProcess above will actually create it.  So the question is
   // do we want to add another process record just to mark it modified at this point?
    if(!created) {
        m_processCxt->updateProcess(&(proc->proc), ev, SFObjectState::MODIFIED);
        cout << "Writing modified process..." << proc->proc.exe << endl;
        m_writer->writeProcess(&(proc->proc));
    }

    m_procFlow.opFlags =  OP_EXEC;
    m_procFlow.ts = ev->get_ts();
    m_procFlow.procOID.hpid = proc->proc.oid.hpid;
    m_procFlow.procOID.createTS = proc->proc.oid.createTS;
    m_procFlow.tid = ti->m_tid;
    m_procFlow.ret = utils::getSyscallResult(ev);
    m_writer->writeProcessFlow(&m_procFlow);
}

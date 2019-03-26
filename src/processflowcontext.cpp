#include "processflowcontext.h"

using namespace processflow;
ProcessFlowContext::ProcessFlowContext(SysFlowWriter* writer, process::ProcessContext* pc) {
    m_processCxt = pc;
    m_writer = writer;
}

ProcessFlowContext::~ProcessFlowContext() {

}

void ProcessFlowContext::writeCloneEvent(sinsp_evt* ev) {
    sinsp_threadinfo* ti = ev->get_thread_info();
    bool created = false;
    Process* proc = m_processCxt->getProcess(ev, ActionType::CREATED, created);
    m_procFlow.type =  CLONE;
    m_procFlow.ts = ev->get_ts();
    m_procFlow.procOID.hpid = proc->oid.hpid;
    m_procFlow.procOID.createTS = proc->oid.createTS;
    m_procFlow.tid = ti->m_tid;
    m_procFlow.ret = utils::getSyscallResult(ev);
    m_writer->writeProcessFlow(&m_procFlow);
}

void ProcessFlowContext::writeExitEvent(sinsp_evt* ev) {
    sinsp_threadinfo* ti = ev->get_thread_info();
    bool created = false;
    Process* proc = m_processCxt->getProcess(ev, ActionType::REUP, created);
    m_procFlow.type =  EXIT;
    m_procFlow.ts = ev->get_ts();
    m_procFlow.procOID.hpid = proc->oid.hpid;
    m_procFlow.procOID.createTS = proc->oid.createTS;
    m_procFlow.tid = ti->m_tid;
    m_procFlow.ret = utils::getSyscallResult(ev);
    m_writer->writeProcessFlow(&m_procFlow);
    // delete the process from the proc table after an exit
    m_processCxt->deleteProcess(&proc);
}

void ProcessFlowContext::writeExecEvent(sinsp_evt* ev) {
    sinsp_threadinfo* ti = ev->get_thread_info();
    bool created = false;
    Process* proc = m_processCxt->getProcess(ev, ActionType::CREATED, created);
   /* if(!created) {
      cout << "Exec on an existing process!" << endl;
    }*/

    //If Clones are filtered out (or a process like bash is filtered out) then we will only see
    // the EXEC of this process, and the getProcess above will actually create it.  So the question is
   // do we want to add another process record just to mark it modified at this point?
    if(!created) {
        m_processCxt->updateProcess(proc, ev, ActionType::MODIFIED);
        cout << "Writing modified process..." << proc->exe << endl;
        m_writer->writeProcess(proc);
    }

    m_procFlow.type =  EXEC;
    m_procFlow.ts = ev->get_ts();
    m_procFlow.procOID.hpid = proc->oid.hpid;
    m_procFlow.procOID.createTS = proc->oid.createTS;
    m_procFlow.tid = ti->m_tid;
    m_procFlow.ret = utils::getSyscallResult(ev);
    m_writer->writeProcessFlow(&m_procFlow);
}

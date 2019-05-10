#include "processeventprocessor.h"

using namespace processevent;
LoggerPtr ProcessEventProcessor::m_logger(Logger::getLogger("sysflow.processevent"));
ProcessEventProcessor::ProcessEventProcessor(SysFlowWriter* writer, process::ProcessContext* pc, dataflow::DataFlowProcessor* dfPrcr) {
    m_processCxt = pc;
    m_writer = writer;
    m_dfPrcr = dfPrcr;
}

ProcessEventProcessor::~ProcessEventProcessor() {

}

void ProcessEventProcessor::setUID(sinsp_evt* ev) {
/*    for(uint32_t i = 0; i < ev->get_num_params(); i ++) {
     string name = ev->get_param_name(i);
     const ppm_param_info* param = ev->get_param_info(i);
          const sinsp_evt_param* p = ev->get_param_value_raw(name.c_str());
         cout << name  << " " << ev->get_param_value_str(name.c_str()) << " " <<  param->type << " " << (uint32_t)param->ninfo <<   endl;
     if(param->type == PT_PID) {
        int64_t pid = *(int64_t *)p->m_val;
        cout << pid << endl;
    }
    }
  */
    m_uid = ev->get_param_value_str(SF_UID);
}

void ProcessEventProcessor::writeCloneEvent(sinsp_evt* ev) {
    sinsp_threadinfo* ti = ev->get_thread_info();
    bool created = false;
    ProcessObj* proc = m_processCxt->getProcess(ev, SFObjectState::CREATED, created);
    m_procEvt.opFlags =  OP_CLONE;
    m_procEvt.ts = ev->get_ts();
    m_procEvt.procOID.hpid = proc->proc.oid.hpid;
    m_procEvt.procOID.createTS = proc->proc.oid.createTS;
    m_procEvt.tid = ti->m_tid;
    m_procEvt.ret = utils::getSyscallResult(ev);
    m_procEvt.args.clear();
    m_writer->writeProcessEvent(&m_procEvt);
}

void ProcessEventProcessor::writeSetUIDEvent(sinsp_evt* ev) {
    sinsp_threadinfo* ti = ev->get_thread_info();
    bool created = false;
    ProcessObj* proc = m_processCxt->getProcess(ev, SFObjectState::REUP, created);
    if(!created) {
        m_processCxt->updateProcess(&(proc->proc), ev, SFObjectState::MODIFIED);
        LOG4CXX_DEBUG(m_logger, "Writing modified process..." << proc->proc.exe);
        m_writer->writeProcess(&(proc->proc));
    }
    m_procEvt.opFlags =  OP_SETUID;
    m_procEvt.ts = ev->get_ts();
    m_procEvt.procOID.hpid = proc->proc.oid.hpid;
    m_procEvt.procOID.createTS = proc->proc.oid.createTS;
    m_procEvt.tid = ti->m_tid;
    m_procEvt.ret = utils::getSyscallResult(ev);
    m_procEvt.args.clear();
    m_procEvt.args.push_back(m_uid);
    m_writer->writeProcessEvent(&m_procEvt);
    m_procEvt.args.clear();
}

void ProcessEventProcessor::writeExitEvent(sinsp_evt* ev) {
    sinsp_threadinfo* ti = ev->get_thread_info();
    bool created = false;
    ProcessObj* proc = m_processCxt->getProcess(ev, SFObjectState::REUP, created);
    m_procEvt.opFlags =  OP_EXIT;
    m_procEvt.ts = ev->get_ts();
    m_procEvt.procOID.hpid = proc->proc.oid.hpid;
    m_procEvt.procOID.createTS = proc->proc.oid.createTS;
    m_procEvt.tid = ti->m_tid;
    m_procEvt.ret = utils::getSyscallResult(ev);
    m_procEvt.args.clear();
    int64_t tid = -1;
    if(!ti->is_main_thread()) {
         tid = ti->m_tid;
    }
    m_dfPrcr->removeAndWriteDFFromProc(proc, tid);
    m_writer->writeProcessEvent(&m_procEvt);
    // delete the process from the proc table after an exit
    if(ti->is_main_thread()) {
        m_processCxt->deleteProcess(&proc);
    }
}

void ProcessEventProcessor::writeExecEvent(sinsp_evt* ev) {
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
        LOG4CXX_DEBUG(m_logger, "Writing modified process..." << proc->proc.exe);
        m_writer->writeProcess(&(proc->proc));
    }

    m_procEvt.opFlags =  OP_EXEC;
    m_procEvt.ts = ev->get_ts();
    m_procEvt.procOID.hpid = proc->proc.oid.hpid;
    m_procEvt.procOID.createTS = proc->proc.oid.createTS;
    m_procEvt.tid = ti->m_tid;
    m_procEvt.ret = utils::getSyscallResult(ev);
    m_procEvt.args.clear();
    m_writer->writeProcessEvent(&m_procEvt);
}

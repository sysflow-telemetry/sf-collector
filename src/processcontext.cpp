#include "processcontext.h"

using namespace process;

ProcessContext::ProcessContext(SysFlowContext* cxt, container::ContainerContext* ccxt, SysFlowWriter* writer) : m_procs(PROC_TABLE_SIZE) {
    m_empkey.hpid = 0; 
    m_empkey.createTS = 0; 
    m_procs.set_empty_key(&m_empkey);
    m_delkey.hpid = 1;
    m_delkey.createTS = 1;
    m_procs.set_deleted_key(&m_delkey);
    m_cxt = cxt;
    m_containerCxt = ccxt;
    m_writer = writer;
}

ProcessContext::~ProcessContext() {
    clearProcesses();
}

Process* ProcessContext::createProcess(sinsp_threadinfo* mainthread, sinsp_evt* ev, ActionType actType) {
   
   Process* proc = new Process();
   proc->type = actType;
   proc->ts = ev->get_ts();
   proc->oid.hpid = mainthread->m_pid;
   proc->oid.createTS = mainthread->m_clone_ts;

   sinsp_threadinfo* parent = mainthread->get_parent_thread();

   if(parent != NULL) {
      OID poid;
      poid.createTS = parent->m_clone_ts;
      poid.hpid = parent->m_pid;
      proc->poid.set_OID(poid);
   }
   //std::memcpy(&proc.oid[0], &proc.ts, sizeof(int64_t));
   //std::memcpy(&proc->oid[0], &mainthread->m_clone_ts, sizeof(int64_t));
   //std::memcpy(&proc->oid[8], &proc->hpid, sizeof(int32_t));
   //cout << "Wrote OID" << endl;
   proc->exe = mainthread->m_exepath;
   proc->exeArgs.clear();
   int i = 0;
   for(std::vector<string>::iterator it = mainthread->m_args.begin(); it != mainthread->m_args.end(); ++it) {
       if (i == 0) {
           proc->exeArgs += *it;
       }else {
           proc->exeArgs +=  " " + *it;
       }
       i++;
   }
   //cout << "Wrote exe args" << endl;
    proc->uid = mainthread->m_uid;
    proc->gid = mainthread->m_gid;
   //cout << "Wrote gid/uid " << ti->m_uid << endl;
   proc->userName = utils::getUserName(m_cxt, mainthread->m_uid);
  // cout << "Wrote username" << endl;
    proc->groupName = utils::getGroupName(m_cxt, mainthread->m_gid);
    Container* cont = m_containerCxt->getContainer(ev);
    if(cont != NULL) {
       proc->containerId.set_string(cont->id);
    }else {
       proc->containerId.set_null();
    }
    //cout << "Wrote user/groupnames" << endl;
    /*if(mainthread->m_clone_ts != 0) {
       proc->duration = ev->get_ts() - mainthread->m_clone_ts;
    }*/
    //proc->childCount = mainthread->m_nchilds;
    return proc;
}

bool ProcessContext::isAncestor(OID* oid, Process* proc) {

   Process::poid_t poid = proc->poid;
  
   while(!poid.is_null()){
       OID key = poid.get_OID();
       ProcessTable::iterator p = m_procs.find(&(key));
       if(p != m_procs.end()) {
          OID o = p->second->oid;
          if(oid->hpid == o.hpid && oid->createTS == o.createTS) {
              return true;
          }
          poid = p->second->poid;
       }else {
           break;
       }
   }

   return false;
}

Process* ProcessContext::getProcess(sinsp_evt* ev, ActionType actType, bool& created) {
      sinsp_threadinfo* ti = ev->get_thread_info();
      sinsp_threadinfo* mt = ti->get_main_thread();
      OID key;
      key.createTS = mt->m_clone_ts;
      key.hpid = mt->m_pid;
      created = true;
      //std::memcpy(&key[0], &mt->m_clone_ts, sizeof(int64_t));
      //std::memcpy(&key[8], &mt->m_pid, sizeof(int32_t));
      ProcessTable::iterator proc = m_procs.find(&key);
      if(proc != m_procs.end()) {
          created = false;
          return proc->second;
      }
      std::vector<Process*> processes; 
      Process* process = createProcess(mt, ev, actType);
      processes.push_back(process);
      mt = mt->get_parent_thread();
      while(mt != NULL) {
          if(mt->m_clone_ts == 0 && mt->m_pid == 0) {
              mt = mt->get_parent_thread();
              continue;
          }
	  key.createTS = mt->m_clone_ts;
	  key.hpid = mt->m_pid;
         // cout << "PID: " << mt->m_pid << " ts " << mt->m_clone_ts <<  " EXE " << mt->m_exepath << endl;
          //std::memcpy(&key[0], &mt->m_clone_ts, sizeof(int64_t));
          //std::memcpy(&key[8], &mt->m_pid, sizeof(int32_t));
          ProcessTable::iterator proc2 = m_procs.find(&key);
          if(proc2 != m_procs.end()) {
	      break;
          }
          Process* parent = createProcess(mt, ev, ActionType::REUP);
          processes.push_back(parent);
          mt = mt->get_parent_thread();
      }

      for(vector<Process*>::reverse_iterator it = processes.rbegin(); it != processes.rend(); ++it) {
          m_procs[&(*it)->oid] = (*it);
          m_writer->writeProcess((*it));
      }
      return process;
}

void ProcessContext::updateProcess(Process* proc, sinsp_evt* ev, ActionType actType) {
   sinsp_threadinfo* ti = ev->get_thread_info();
   sinsp_threadinfo* mainthread = ti->get_main_thread();
   proc->type = actType;
   proc->ts = ev->get_ts();
   proc->exe = mainthread->m_exepath;
   proc->exeArgs.clear();
   int i = 0;
   for(std::vector<string>::iterator it = mainthread->m_args.begin(); it != mainthread->m_args.end(); ++it) {
       if (i == 0) {
           proc->exeArgs += *it;
       }else {
           proc->exeArgs +=  " " + *it;
       }
       i++;
   }
    proc->uid = mainthread->m_uid;
    proc->gid = mainthread->m_gid;
   //cout << "Wrote gid/uid " << ti->m_uid << endl;
   proc->userName = utils::getUserName(m_cxt, mainthread->m_uid);
  // cout << "Wrote username" << endl;
   proc->groupName = utils::getGroupName(m_cxt, mainthread->m_gid);

}

void ProcessContext::clearProcesses() {
    for(ProcessTable::iterator it = m_procs.begin(); it != m_procs.end(); ++it) {
        delete it->second;
    }
    m_procs.clear();
}

void ProcessContext::deleteProcess(Process** proc) {
    m_procs.erase(&((*proc)->oid));
    delete *proc; 
    *proc = NULL;
}



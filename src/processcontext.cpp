#include "processcontext.h"

using namespace process;

ProcessContext::ProcessContext(SysFlowContext* cxt, container::ContainerContext* ccxt, SysFlowWriter* writer) : m_procs(PROC_TABLE_SIZE) {
    m_empkey.hpid = 2; 
    m_empkey.createTS = 2; 
    m_procs.set_empty_key(&m_empkey);
    m_delkey.hpid = 1;
    m_delkey.createTS = 1;
    m_procs.set_deleted_key(&m_delkey);
    m_cxt = cxt;
    m_containerCxt = ccxt;
    m_writer = writer;
}

ProcessContext::~ProcessContext() {
    clearAllProcesses();
}

ProcessObj* ProcessContext::createProcess(sinsp_threadinfo* mainthread, sinsp_evt* ev, SFObjectState state) {
   
   ProcessObj* p = new ProcessObj();
   p->proc.state = state;
   p->proc.ts = ev->get_ts();
   p->proc.oid.hpid = mainthread->m_pid;
   p->proc.oid.createTS = mainthread->m_clone_ts;
   

  sinsp_threadinfo* ti = ev->get_thread_info();

   sinsp_threadinfo* parent = mainthread->get_parent_thread();

   if(parent != NULL) {
      OID poid;
      poid.createTS = parent->m_clone_ts;
      poid.hpid = parent->m_pid;
      p->proc.poid.set_OID(poid);
   }
   //std::memcpy(&proc.oid[0], &proc.ts, sizeof(int64_t));
   //std::memcpy(&proc->oid[0], &mainthread->m_clone_ts, sizeof(int64_t));
   //std::memcpy(&proc->oid[8], &proc->hpid, sizeof(int32_t));
   //cout << "Wrote OID" << endl;
   p->proc.exe = (mainthread->m_exepath.empty()) ? mainthread->m_exe : mainthread->m_exepath;
   cout << "The exepath is " << p->proc.exe <<  " ti: " << ti->get_exepath() << " EXE: " << mainthread->get_exe() << endl;
   p->proc.exeArgs.clear();
   int i = 0;
   for(std::vector<string>::iterator it = mainthread->m_args.begin(); it != mainthread->m_args.end(); ++it) {
       if (i == 0) {
           p->proc.exeArgs += *it;
       }else {
           p->proc.exeArgs +=  " " + *it;
       }
       i++;
   }
   //cout << "Wrote exe args" << endl;
    p->proc.uid = mainthread->m_uid;
    p->proc.gid = mainthread->m_gid;
   //cout << "Wrote gid/uid " << ti->m_uid << endl;
   p->proc.userName = utils::getUserName(m_cxt, mainthread->m_uid);
  // cout << "Wrote username" << endl;
    p->proc.groupName = utils::getGroupName(m_cxt, mainthread->m_gid);
    Container* cont = m_containerCxt->getContainer(ev);
    if(cont != NULL) {
       p->proc.containerId.set_string(cont->id);
    }else {
       p->proc.containerId.set_null();
    }
    //cout << "Wrote user/groupnames" << endl;
    /*if(mainthread->m_clone_ts != 0) {
       proc->duration = ev->get_ts() - mainthread->m_clone_ts;
    }*/
    //proc->childCount = mainthread->m_nchilds;
    return p;
}

void ProcessContext::printAncestors(Process* proc) {
   Process::poid_t poid = proc->poid;
  
   while(!poid.is_null()){
       OID key = poid.get_OID();
       ProcessTable::iterator p = m_procs.find(&(key));
       if(p != m_procs.end()) {
          poid = p->second->proc.poid;
          cout << "-->" << p->second->proc.oid.hpid << " " << p->second->proc.oid.createTS << " " << p->second->proc.exe << " " << p->second->proc.exeArgs << endl;
       }else {
           break;
       }
   }

}



bool ProcessContext::isAncestor(OID* oid, Process* proc) {

   Process::poid_t poid = proc->poid;
  
   while(!poid.is_null()){
       OID key = poid.get_OID();
       ProcessTable::iterator p = m_procs.find(&(key));
       if(p != m_procs.end()) {
          OID o = p->second->proc.oid;
          if(oid->hpid == o.hpid && oid->createTS == o.createTS) {
              return true;
          }
          poid = p->second->proc.poid;
       }else {
           break;
       }
   }

   return false;
}

ProcessObj* ProcessContext::getProcess(sinsp_evt* ev, SFObjectState state, bool& created) {
      sinsp_threadinfo* ti = ev->get_thread_info();
      sinsp_threadinfo* mt = ti->get_main_thread();
      OID key;
      key.createTS = mt->m_clone_ts;
      key.hpid = mt->m_pid;
      created = true;
      cout << "PID: " << mt->m_pid << " ts " << mt->m_clone_ts <<  " EXEPATH: " << mt->m_exepath << " EXE: " << mt->m_exe << endl;
      //std::memcpy(&key[0], &mt->m_clone_ts, sizeof(int64_t));
      //std::memcpy(&key[8], &mt->m_pid, sizeof(int32_t));
      ProcessTable::iterator proc = m_procs.find(&key);
      ProcessObj* process = NULL;
      if(proc != m_procs.end()) {
          created = false;
          if(proc->second->written) {
               return proc->second;
           }
           process = proc->second;
           process->proc.state = SFObjectState::REUP;
      }
      std::vector<ProcessObj*> processes;
      if(process == NULL) { 
           process = createProcess(mt, ev, state);
       }
      cout << "CREATING PROCESS FOR WRITING: PID: " << mt->m_pid << " ts " << mt->m_clone_ts <<  " EXEPATH: " << mt->m_exepath << " EXE: " << mt->m_exe << endl;
      processes.push_back(process);
      mt = mt->get_parent_thread();
      while(mt != NULL) {
          if(mt->m_clone_ts == 0 && mt->m_pid == 0) {
              mt = mt->get_parent_thread();
              continue;
          }
	  key.createTS = mt->m_clone_ts;
	  key.hpid = mt->m_pid;
         cout << "PARENT PID: " << mt->m_pid << " ts " << mt->m_clone_ts <<  " EXEPATH: " << mt->m_exepath << " EXE: " << mt->m_exe << endl;
          //std::memcpy(&key[0], &mt->m_clone_ts, sizeof(int64_t));
          //std::memcpy(&key[8], &mt->m_pid, sizeof(int32_t));
         ProcessObj* parent = NULL;
          ProcessTable::iterator proc2 = m_procs.find(&key);
          if(proc2 != m_procs.end()) {
              if(proc2->second->written) {
	         break;
              } else {
                 parent = proc2->second;
                 parent->proc.state = SFObjectState::REUP;
              }
  
          }
          if(parent == NULL) {
              parent = createProcess(mt, ev, SFObjectState::REUP);
          } 
          processes.push_back(parent);
          mt = mt->get_parent_thread();
      }

      for(vector<ProcessObj*>::reverse_iterator it = processes.rbegin(); it != processes.rend(); ++it) {
          cout << "Going to write process " << (*it)->proc.exe << " " << (*it)->proc.oid.hpid << endl;
          m_procs[&((*it)->proc.oid)] = (*it);
          m_writer->writeProcess(&((*it)->proc));
          (*it)->written = true;
      }
      cout << "Returning process " << endl;
      return process;
}

ProcessObj* ProcessContext::getProcess(OID* oid) {
      ProcessTable::iterator proc = m_procs.find(oid);
      if(proc != m_procs.end()) {
          return proc->second;
      }
      return NULL;
}

bool ProcessContext::exportProcess(OID* oid) {
    ProcessObj* p = getProcess(oid);
    bool expt = false;
    if(p == NULL) {
        cout << "Uh oh!!!  Can't find process! for oid: " << oid->hpid << " " << oid->createTS << endl;
        return expt;
    }
    if(!p->written){
          m_writer->writeProcess(&(p->proc));
          p->written = true;
          expt = true;
    }
    return expt;
}


void ProcessContext::updateProcess(Process* proc, sinsp_evt* ev, SFObjectState state) {
   sinsp_threadinfo* ti = ev->get_thread_info();
   sinsp_threadinfo* mainthread = ti->get_main_thread();
   proc->state = state;
   proc->ts = ev->get_ts();
   //proc->exe = mainthread->m_exepath;
   proc->exe = (mainthread->m_exepath.empty()) ? mainthread->m_exe : mainthread->m_exepath;
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
        if(it->second->netflows.empty() && it->second->fileflows.empty()) {
               ProcessObj* proc = it->second;
               m_procs.erase(it);
               delete proc;
        }else {
             it->second->written = false;
        }
    }
}

void ProcessContext::clearAllProcesses() {
    for(ProcessTable::iterator it = m_procs.begin(); it != m_procs.end(); ++it) {
        if(((!it->second->netflows.empty()) || (!it->second->fileflows.empty())) &&
               !it->second->written) {
            m_writer->writeProcess(&(it->second->proc));
            it->second->written = true;
         }
         for(NetworkFlowTable::iterator nfi = it->second->netflows.begin(); nfi != it->second->netflows.end(); nfi++) {
             nfi->second->netflow.opFlags |= OP_TRUNCATE;
             nfi->second->netflow.endTs = utils::getSysdigTime(m_cxt);
             m_writer->writeNetFlow(&(nfi->second->netflow));
             delete nfi->second;
          }
         /*for(FileFlowTable::iterator ffi = it->second->fileflows.begin(); ffi != it->second->fileflows.end(); ffi++) {
             ffi->second->fileflow.opFlags |= OP_TRUNCATE;
             ffi->second->fileflow.endTs = utils::getSysdigTime(m_cxt);
             m_writer->writeFileFlow(&(ffi->second->fileflow));
             delete ffi->second;
          }*/
          delete it->second;
    }
}




void ProcessContext::deleteProcess(ProcessObj** proc) {
    m_procs.erase(&((*proc)->proc.oid));
    delete *proc; 
    *proc = NULL;
}



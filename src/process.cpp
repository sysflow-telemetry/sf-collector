#include "process.h"

Process* createProcess(Context* cxt, sinsp_threadinfo* mainthread, sinsp_evt* ev, ActionType actType) {
   
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
   proc->userName = getUserName(cxt, mainthread->m_uid);
  // cout << "Wrote username" << endl;
    proc->groupName = getGroupName(cxt, mainthread->m_gid);
    Container* cont = getContainer(cxt, ev);
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
Process* getProcess(Context* cxt, sinsp_evt* ev, ActionType actType, bool& created) {
      sinsp_threadinfo* ti = ev->get_thread_info();
      sinsp_threadinfo* mt = ti->get_main_thread();
      OID key;
      key.createTS = mt->m_clone_ts;
      key.hpid = mt->m_pid;
      created = true;
      //std::memcpy(&key[0], &mt->m_clone_ts, sizeof(int64_t));
      //std::memcpy(&key[8], &mt->m_pid, sizeof(int32_t));
      ProcessTable::iterator proc = cxt->procs.find(&key);
      if(proc != cxt->procs.end()) {
          created = false;
          return proc->second;
      }
      std::vector<Process*> processes; 
      Process* process = createProcess(cxt, mt, ev, actType);
      processes.push_back(process);
      mt = mt->get_parent_thread();
      while(mt != NULL) {
	  key.createTS = mt->m_clone_ts;
	  key.hpid = mt->m_pid;
          //std::memcpy(&key[0], &mt->m_clone_ts, sizeof(int64_t));
          //std::memcpy(&key[8], &mt->m_pid, sizeof(int32_t));
          ProcessTable::iterator proc2 = cxt->procs.find(&key);
          if(proc2 != cxt->procs.end()) {
	      break;
          }
          Process* parent = createProcess(cxt, mt, ev, ActionType::REUP);
          processes.push_back(parent);
          mt = mt->get_parent_thread();
      }

      for(vector<Process*>::reverse_iterator it = processes.rbegin(); it != processes.rend(); ++it) {
          cxt->procs[&(*it)->oid] = (*it);
          writeProcess(cxt, (*it));
      }
      return process;
}

void updateProcess(Context* cxt, Process* proc, sinsp_evt* ev, ActionType actType) {
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
   proc->userName = getUserName(cxt, mainthread->m_uid);
  // cout << "Wrote username" << endl;
   proc->groupName = getGroupName(cxt, mainthread->m_gid);

}



void writeProcess(Context* cxt, Process* proc) {
    cxt->flow.rec.set_Process(*proc);
    cxt->numRecs++;
    cxt->dfw->write(cxt->flow);
    //avro::encode(*encoder, flow);
}

#include "atfileflowprocessor.h"

using namespace atomicflow;


AtomicFileFlowProcessor::AtomicFileFlowProcessor(SysFlowWriter* writer, process::ProcessContext* procCxt, file::FileContext* fileCxt) { 
    m_writer = writer;
    m_processCxt = procCxt;
    m_fileCxt = fileCxt;
}

AtomicFileFlowProcessor::~AtomicFileFlowProcessor() {

}

int AtomicFileFlowProcessor::handleFileFlowEvent(sinsp_evt* ev, OpFlags flag) {
   int res = 1;
   if(flag == OP_MKDIR || flag == OP_RMDIR || flag == OP_UNLINK) {
         res = writeFileEvent(ev, flag);
   } else if(flag == OP_LINK || flag == OP_SYMLINK) {
         res = writeLinkEvent(ev, flag);
   }

   return res;
}


int AtomicFileFlowProcessor::writeLinkEvent(sinsp_evt* ev, OpFlags flag) {
    sinsp_threadinfo* ti = ev->get_thread_info();
    sinsp_threadinfo* mt = ti->get_main_thread();
    bool created = false;
    ProcessObj* proc = m_processCxt->getProcess(ev, SFObjectState::REUP, created);
//    sinsp_fdinfo_t * fdinfo = ev->get_fd_info();
    FileObj* file1 = NULL;
    FileObj* file2 = NULL;
  /*  if(fdinfo != NULL) {
        file = m_fileCxt->getFile(ev, SFObjectState::CREATED, created);
    } else {*/
        string path1; 
        string path2; 
        if(flag == OP_LINK) {
            path1 =  ev->get_param_value_str("oldpath");
            path2 =  ev->get_param_value_str("newpath"); 
            cout << "PATH2: " << ev->get_param_value_str("newpath") << endl;
        } else if(flag == OP_SYMLINK) {
            path1 =  ev->get_param_value_str("target");
            path2 =  ev->get_param_value_str("linkpath");
            cout << "PATH2: " << ev->get_param_value_str("linkpath") << endl;
        }
        cout << ev->get_name() << " path1: " << path1 << " path2: " << path2 << endl;

        file1 = m_fileCxt->getFile(ev, path1, 'f', SFObjectState::REUP, created);
        file2 = m_fileCxt->getFile(ev, path2, 'f', SFObjectState::CREATED, created);
   // }
    m_atFileFlow.opFlags = flag;
    m_atFileFlow.ts = ev->get_ts();
    m_atFileFlow.procOID.hpid = proc->proc.oid.hpid;
    m_atFileFlow.procOID.createTS = proc->proc.oid.createTS;
    m_atFileFlow.tid = ti->m_tid;
    m_atFileFlow.ret = utils::getSyscallResult(ev);
    m_atFileFlow.fd = ev->get_fd_num();
    m_atFileFlow.fileOID = file1->file.oid;
    m_atFileFlow.newFileOID.set_FOID(file2->file.oid);
    for(uint32_t i = 0; i < ev->get_num_params(); i ++) {
     string name = ev->get_param_name(i);
     const ppm_param_info* param = ev->get_param_info(i);
          const sinsp_evt_param* p = ev->get_param_value_raw(name.c_str());
         cout << name  << " " << ev->get_param_value_str(name.c_str()) << " " <<  param->type << " " << (uint32_t)param->ninfo <<   endl;
     if(param->type == PT_PID) {
        int64_t pid = *(int64_t *)p->m_val;
        cout << pid << endl;
    }
    }
    cout << "The Current working Directory of the mkdir event is " << mt->get_cwd() << endl;
    m_writer->writeAtomicFileFlow(&m_atFileFlow);
    return 0;
}

int AtomicFileFlowProcessor::writeFileEvent(sinsp_evt* ev, OpFlags flag) {
    sinsp_threadinfo* ti = ev->get_thread_info();
    sinsp_threadinfo* mt = ti->get_main_thread();
    bool created = false;
    ProcessObj* proc = m_processCxt->getProcess(ev, SFObjectState::REUP, created);
    sinsp_fdinfo_t * fdinfo = ev->get_fd_info();
    FileObj* file = NULL;
    if(fdinfo != NULL) {
        file = m_fileCxt->getFile(ev, SFObjectState::CREATED, created);
    } else {
        string path =  ev->get_param_value_str("path");
        file = m_fileCxt->getFile(ev, path, 'd', SFObjectState::CREATED, created);
    }
    m_atFileFlow.opFlags = flag;
    m_atFileFlow.ts = ev->get_ts();
    m_atFileFlow.procOID.hpid = proc->proc.oid.hpid;
    m_atFileFlow.procOID.createTS = proc->proc.oid.createTS;
    m_atFileFlow.tid = ti->m_tid;
    m_atFileFlow.ret = utils::getSyscallResult(ev);
    m_atFileFlow.fd = ev->get_fd_num();
    m_atFileFlow.fileOID = file->file.oid;
    m_atFileFlow.newFileOID.set_null();
    for(uint32_t i = 0; i < ev->get_num_params(); i ++) {
     string name = ev->get_param_name(i);
     const ppm_param_info* param = ev->get_param_info(i);
          const sinsp_evt_param* p = ev->get_param_value_raw(name.c_str());
         cout << name  << " " << ev->get_param_value_str(name.c_str()) << " " <<  param->type << " " << (uint32_t)param->ninfo <<   endl;
     if(param->type == PT_PID) {
        int64_t pid = *(int64_t *)p->m_val;
        cout << pid << endl;
    }
    }
    cout << "The Current working Directory of the mkdir event is " << mt->get_cwd() << endl;
    m_writer->writeAtomicFileFlow(&m_atFileFlow);
    return 0;
}

#include "fileeventprocessor.h"
#include <assert.h>

using namespace fileevent;


FileEventProcessor::FileEventProcessor(SysFlowWriter* writer, process::ProcessContext* procCxt, file::FileContext* fileCxt) { 
    m_writer = writer;
    m_processCxt = procCxt;
    m_fileCxt = fileCxt;
}

FileEventProcessor::~FileEventProcessor() {

}

int FileEventProcessor::handleFileFlowEvent(sinsp_evt* ev, OpFlags flag) {
   int res = 1;
   if(flag == OP_MKDIR || flag == OP_RMDIR || flag == OP_UNLINK) {
         res = writeFileEvent(ev, flag);
   } else if(flag == OP_LINK || flag == OP_SYMLINK || flag == OP_RENAME) {
         res = writeLinkEvent(ev, flag);
   }

   return res;
}


int FileEventProcessor::writeLinkEvent(sinsp_evt* ev, OpFlags flag) {
    sinsp_threadinfo* ti = ev->get_thread_info();
    //sinsp_threadinfo* mt = ti->get_main_thread();
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
        if(flag == OP_LINK || flag == OP_RENAME) {
            path1 =  utils::getPath(ev, "oldpath");
            path2 =  utils::getPath(ev, "newpath");
            cout << "Getting paths: " <<  path1 << " Path2: " << endl;
            if(IS_AT_SC(ev->get_type())) {
                int64_t olddirfd;
                int64_t newdirfd;
                if(flag == OP_RENAME) {
                    olddirfd = utils::getFD(ev, "olddirfd");
                    newdirfd = utils::getFD(ev, "newdirfd");
                } else {
                    olddirfd = utils::getFD(ev, "olddir");
                    newdirfd = utils::getFD(ev, "newdir");
                }
                path1 = utils::getAbsolutePath(ti, olddirfd, path1);
                path2 = utils::getAbsolutePath(ti, newdirfd, path2);
          
            } else {
                path1 = utils::getAbsolutePath(ti, path1);
                path2 = utils::getAbsolutePath(ti, path2);
            }
            cout << "PATH2: " << path2 << endl;
        } else if(flag == OP_SYMLINK) {
            path1 =  utils::getPath(ev, "target");
            path2 =  utils::getPath(ev, "linkpath");
            if(IS_AT_SC(ev->get_type())) {
                uint64_t linkdirfd = utils::getFD(ev, "linkdirfd");
                path2 = utils::getAbsolutePath(ti, linkdirfd, path2);
            } else {
                path1 = utils::getAbsolutePath(ti, path1);
                path2 = utils::getAbsolutePath(ti, path2);
            }
            cout << "PATH2: " << path2 << endl;
        }
        cout << ev->get_name() << " path1: " << path1 << " path2: " << path2 << endl;

        file1 = m_fileCxt->getFile(ev, path1, SF_UNK, SFObjectState::REUP, created);
        file2 = m_fileCxt->getFile(ev, path2, SF_UNK, SFObjectState::CREATED, created);
   // }
    m_fileEvt.opFlags = flag;
    m_fileEvt.ts = ev->get_ts();
    m_fileEvt.procOID.hpid = proc->proc.oid.hpid;
    m_fileEvt.procOID.createTS = proc->proc.oid.createTS;
    m_fileEvt.tid = ti->m_tid;
    m_fileEvt.ret = utils::getSyscallResult(ev);
    //m_fileEvt.fd = ev->get_fd_num();
    m_fileEvt.fileOID = file1->file.oid;
    m_fileEvt.newFileOID.set_FOID(file2->file.oid);
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
    cout << "The Current working Directory of the mkdir event is " << ti->get_cwd() << endl;
    m_writer->writeFileEvent(&m_fileEvt);
    return 0;
}

int FileEventProcessor::writeFileEvent(sinsp_evt* ev, OpFlags flag) {
    sinsp_threadinfo* ti = ev->get_thread_info();
    //sinsp_threadinfo* mt = ti->get_main_thread();
    bool created = false;
    ProcessObj* proc = m_processCxt->getProcess(ev, SFObjectState::REUP, created);
    sinsp_fdinfo_t * fdinfo = ev->get_fd_info();
    FileObj* file = NULL;
    if(fdinfo != NULL) {
        file = m_fileCxt->getFile(ev, SFObjectState::CREATED, created);
    } else {
        string fileName = (IS_UNLINKAT(ev->get_type())) ? utils::getPath(ev, "name") : utils::getPath(ev, "path");
        if(IS_AT_SC(ev->get_type())) {
            sinsp_evt_param *pinfo;
            pinfo = ev->get_param(1);
            assert(pinfo->m_len == sizeof(int64_t));
            int64_t dirfd = *(int64_t *)pinfo->m_val;
            fileName = utils::getAbsolutePath(ti, dirfd, fileName);
        }else {
            fileName = utils::getAbsolutePath(ti, fileName);
        }
        file = m_fileCxt->getFile(ev, fileName, SF_UNK, SFObjectState::CREATED, created);
    }
    m_fileEvt.opFlags = flag;
    m_fileEvt.ts = ev->get_ts();
    m_fileEvt.procOID.hpid = proc->proc.oid.hpid;
    m_fileEvt.procOID.createTS = proc->proc.oid.createTS;
    m_fileEvt.tid = ti->m_tid;
    m_fileEvt.ret = utils::getSyscallResult(ev);
    //m_fileEvt.fd = ev->get_fd_num();
    m_fileEvt.fileOID = file->file.oid;
    m_fileEvt.newFileOID.set_null();
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
    cout << "The Current working Directory of the mkdir event is " << ti->get_cwd() << endl;
    m_writer->writeFileEvent(&m_fileEvt);
    return 0;
}

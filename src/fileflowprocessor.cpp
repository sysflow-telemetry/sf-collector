#include "fileflowprocessor.h"
#include "utils.h"
using namespace fileflow;


FileFlowProcessor::FileFlowProcessor(SysFlowContext* cxt, SysFlowWriter* writer, process::ProcessContext* processCxt, DataFlowSet* dfSet, file::FileContext* fileCxt)  {
    m_cxt = cxt;
    m_writer = writer;
    m_processCxt = processCxt;
    m_dfSet = dfSet;
    m_fileCxt = fileCxt;
}

FileFlowProcessor::~FileFlowProcessor() {
}

inline void FileFlowProcessor::populateFileFlow(FileFlowObj* ff, OpFlags flag, sinsp_evt* ev, ProcessObj* proc, FileObj* file) {
   sinsp_threadinfo* ti = ev->get_thread_info();
   ff->fileflow.opFlags = flag;
   ff->fileflow.ts = ev->get_ts();
   ff->fileflow.endTs = 0;
   ff->fileflow.procOID.hpid = proc->proc.oid.hpid;
   ff->fileflow.procOID.createTS = proc->proc.oid.createTS;
   ff->fileflow.tid = ti->m_tid;
   ff->fileflow.fd = ev->get_fd_num();
   ff->fileflow.fileOID = file->file.oid;
   ff->key = file->key;
   ff->fileflow.numRRecvOps = 0;
   ff->fileflow.numWSendOps = 0;
   ff->fileflow.numRRecvBytes = 0;
   ff->fileflow.numWSendBytes = 0;
}

inline void FileFlowProcessor::updateFileFlow(FileFlowObj* ff, OpFlags flag, sinsp_evt* ev) {
       ff->fileflow.opFlags |= flag;
       ff->lastUpdate = utils::getCurrentTime(m_cxt);
       if(flag == OP_WRITE_SEND) {
           ff->fileflow.numWSendOps++;
           int res = utils::getSyscallResult(ev);
           if(res > 0 ) {
               ff->fileflow.numWSendBytes+= res;
           }
       }else if(flag == OP_READ_RECV) {
           ff->fileflow.numRRecvOps++;
           int res = utils::getSyscallResult(ev);
           if(res > 0 ) {
               ff->fileflow.numRRecvBytes+= res;
           }
       }
}

inline void FileFlowProcessor::processNewFlow(sinsp_evt* ev, ProcessObj* proc, FileObj* file, OpFlags flag, string key) {
    FileFlowObj* ff = new FileFlowObj();
    ff->exportTime = utils::getExportTime(m_cxt); 
    ff->lastUpdate = utils::getCurrentTime(m_cxt);
    populateFileFlow(ff, flag, ev, proc, file);
    updateFileFlow(ff, flag, ev);
    if(flag != OP_CLOSE) {
         proc->fileflows[key] = ff;
         file->refs++;
        //m_fileflows[key] = nf;
        m_dfSet->insert(ff);
     }else {
        ff->fileflow.endTs = ev->get_ts();
        m_writer->writeFileFlow(&(ff->fileflow));
        delete ff;
     }
}

inline void FileFlowProcessor::removeAndWriteFileFlow(ProcessObj* proc, FileObj* file,  FileFlowObj** ff, string key) {
    m_writer->writeFileFlow(&((*ff)->fileflow));
    //m_fileflows.erase(*key);
    //m_nfSet.erase((*nf));
    removeFileFlowFromSet(ff, false);
    removeFileFlow(proc, file, ff, key);
}


inline void FileFlowProcessor::processExistingFlow(sinsp_evt* ev, ProcessObj* proc, FileObj* file, OpFlags flag, string key, FileFlowObj* ff) {
      updateFileFlow(ff, flag, ev);
      if(flag == OP_CLOSE) {
          ff->fileflow.endTs = ev->get_ts();
          removeAndWriteFileFlow(proc, file, &ff, key);
      }
}

int FileFlowProcessor::handleFileFlowEvent(sinsp_evt* ev, OpFlags flag) {
    sinsp_fdinfo_t * fdinfo = ev->get_fd_info();
    if(fdinfo == NULL) {
       cout << "Uh oh!!! Event: " << ev->get_name() << " doesn't have an fdinfo associated with it! " << endl;
       return 1;
    }

    if((fdinfo->is_ipv4_socket() || fdinfo->is_ipv6_socket())) {
        cout << "handleFileFlowEvent cannot handle ip sockets,  Ignoring.." << endl;
        return 1;
    }
    char restype = fdinfo->get_typechar();

    switch(restype) {
        case SF_FILE:
        case SF_UNIX:
        case SF_PIPE:
            break;
        default:
             return 1;
    }
    bool created = false;
    //calling get process is important because it ensures that the process object has been written to the
    //sysflow file.   This is important for long running NetworkFlows that may span across files.
    ProcessObj* proc = m_processCxt->getProcess(ev, SFObjectState::REUP, created);
    FileFlowObj* ff = NULL;
    sinsp_threadinfo* ti = ev->get_thread_info();
    string key =  ti->m_container_id + fdinfo->m_name;
    FileFlowTable::iterator ffi = proc->fileflows.find(key);
    if(ffi != proc->fileflows.end()) {
         ff = ffi->second;
    }
    FileObj* file = m_fileCxt->getFile(ev, SFObjectState::REUP, created);
    cout << proc->proc.exe << " Name: " <<  fdinfo->m_name << " type: " << fdinfo->get_typechar() <<  " " << file->file.path << " " << endl; //file->file.oid <<  endl;

    if(ff == NULL) {
       processNewFlow(ev, proc, file, flag, key);
    }else {
       processExistingFlow(ev, proc, file, flag, key, ff);
    }
    return 0;
}

void FileFlowProcessor::removeFileFlow(ProcessObj* proc, FileObj* file,  FileFlowObj** ff, string key) {
    proc->fileflows.erase(key);
    delete *ff;
    ff = NULL;
    if(file != NULL) {
        file->refs--;
    }
}

int FileFlowProcessor::removeAndWriteFFFromProc(ProcessObj* proc, int64_t tid) {
    cout << "CALLING removeANDWRITEFFFlows" << endl;
    int deleted = 0;
    for(FileFlowTable::iterator ffi = proc->fileflows.begin(); ffi != proc->fileflows.end(); ffi++) {
        if(tid == -1 || tid == ffi->second->fileflow.tid) {
            FileObj* file = m_fileCxt->getFile(ffi->second->key);
            ffi->second->fileflow.endTs = utils::getSysdigTime(m_cxt);
            ffi->second->fileflow.opFlags |= OP_TRUNCATE;
            cout << "Writing FILEFLOW!!" << endl;
            m_writer->writeFileFlow(&(ffi->second->fileflow));
            FileFlowObj* ffo = ffi->second;
            cout << "Set size: " << m_dfSet->size() << endl;
            deleted += removeFileFlowFromSet(&ffo, true);
            cout << "After Set size: " << m_dfSet->size() << endl;
            if(file == NULL) {
                cout << "Error: Uh oh! File object doesn't exist for fileflow: " << ffi->second->key << ". This shouldn't happen" << endl;
            }else {
                file->refs--;
            }
        }

    }
    if(tid == -1) {
       proc->fileflows.clear();
    }
    return deleted;
}

int FileFlowProcessor::removeFileFlowFromSet(FileFlowObj** ffo, bool deleteFileFlow) {
        bool found = false;
        int removed = 0;
        for(DataFlowSet::iterator iter = m_dfSet->find(*ffo); iter != m_dfSet->end(); iter++) {
            if(!((*iter)->isNetworkFlow)) {
                FileFlowObj* foundObj = static_cast<FileFlowObj*>(*iter);
                //cout << "Found: " << foundObj->fileflow.procOID.createTS << " " << foundObj->fileflow.procOID.hpid << endl;
                if(*foundObj == **ffo) {
                     cout << "Removing element from multiset" << endl;
                     m_dfSet->erase(iter);
                     if(deleteFileFlow) {
                        delete *ffo;
                        ffo = NULL;
                     }
                     removed++;
                     found = true;
                     break;   
               }
           }
        }
        if(!found) {
           cout << "Error: Cannot find FileFlow Object in data flow set. Deleting. This should not happen." << endl;
           if(deleteFileFlow) {
               delete *ffo;
               ffo = NULL;
           }
        }
        return removed;
}

void FileFlowProcessor::removeFileFlow(DataFlowObj* dfo) {
     FileFlowObj* ffo = static_cast<FileFlowObj*>(dfo);
     //do we want to write out a fileflow that hasn't had any action in an interval?
     //nfo->fileflow.endTs = utils::getSysdigTime(m_cxt);
     //m_writer->writeNetFlow(&(nfo->fileflow));
     cout << "Erasing flow!!! " << endl;
     ProcessObj* proc = m_processCxt->getProcess(&(ffo->fileflow.procOID));
     if(proc == NULL) {
         cout << "Error: Could not find proc " << ffo->fileflow.procOID.hpid << " " << ffo->fileflow.procOID.createTS << " This shouldn't happen!" << endl;
     } else {
          FileObj* file = m_fileCxt->getFile(ffo->key);
          if(file == NULL) {
             cout << "Error: Uh oh!!  Unable to find file object of key " << ffo->key  << ". Shouldn't happen!!" << endl;
          }
          removeFileFlow(proc, file, &ffo, ffo->key);
     }
}

void FileFlowProcessor::exportFileFlow(DataFlowObj* dfo, time_t now) {
     FileFlowObj* ffo = static_cast<FileFlowObj*>(dfo);
     ffo->fileflow.endTs = utils::getSysdigTime(m_cxt);
     m_processCxt->exportProcess(&(ffo->fileflow.procOID));
     m_fileCxt->exportFile(ffo->key);
     m_writer->writeFileFlow(&(ffo->fileflow));
     cout << "Reupping flow!!! " << endl;
     ffo->fileflow.ts = utils::getSysdigTime(m_cxt);
     ffo->fileflow.endTs = 0;
     ffo->fileflow.opFlags = 0;
     ffo->fileflow.numRRecvOps = 0;
     ffo->fileflow.numWSendOps = 0;
     ffo->fileflow.numRRecvBytes = 0;
     ffo->fileflow.numWSendBytes = 0;
}

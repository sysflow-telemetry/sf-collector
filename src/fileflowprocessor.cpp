#include "fileflowprocessor.h"
#include "utils.h"
using namespace fileflow;

CREATE_LOGGER(FileFlowProcessor, "sysflow.fileflow");

FileFlowProcessor::FileFlowProcessor(SysFlowContext* cxt, SysFlowWriter* writer, process::ProcessContext* processCxt, DataFlowSet* dfSet, file::FileContext* fileCxt)  {
    m_cxt = cxt;
    m_writer = writer;
    m_processCxt = processCxt;
    m_dfSet = dfSet;
    m_fileCxt = fileCxt;
}

FileFlowProcessor::~FileFlowProcessor() {
}

inline void FileFlowProcessor::populateFileFlow(/*sinsp_fdinfo_t * fdinfo,*/ FileFlowObj* ff, OpFlags flag, sinsp_evt* ev, ProcessObj* proc, FileObj* file, string flowkey) {
   sinsp_threadinfo* ti = ev->get_thread_info();
   sinsp_fdinfo_t * fdinfo = ev->get_fd_info();
   ff->fileflow.opFlags = flag;
   ff->fileflow.ts = ev->get_ts();
   ff->fileflow.openFlags = 0;
   if(flag == OP_OPEN) {
       ff->fileflow.openFlags = fdinfo->m_openflags;
   }
   ff->fileflow.endTs = 0;
   ff->fileflow.procOID.hpid = proc->proc.oid.hpid;
   ff->fileflow.procOID.createTS = proc->proc.oid.createTS;
   ff->fileflow.tid = ti->m_tid;
   ff->fileflow.fd = ev->get_fd_num();
   ff->fileflow.fileOID = file->file.oid;
   ff->filekey = file->key;
   ff->flowkey = flowkey;
   ff->fileflow.numRRecvOps = 0;
   ff->fileflow.numWSendOps = 0;
   ff->fileflow.numRRecvBytes = 0;
   ff->fileflow.numWSendBytes = 0;
}

void FileFlowProcessor::removeAndWriteRelatedFlows(ProcessObj* proc, FileFlowObj* ffo, uint64_t endTs) {
    vector<FileFlowObj*> ffobjs;
    for(FileFlowTable::iterator ffi = proc->fileflows.begin(); ffi != proc->fileflows.end(); ffi++) {
	if(ffi->second->fileflow.tid != ffo->fileflow.tid &&  ffi->second->fileflow.fd == ffo->fileflow.fd
			&& ffi->second->filekey.compare(ffo->filekey) == 0) {
		if(ffi->second->fileflow.opFlags & OP_OPEN) {
		    ffobjs.insert(ffobjs.begin(), ffi->second);
		} else {
		    ffobjs.push_back(ffi->second);
		}
	     SF_DEBUG(m_logger, "Removing related file flow on thread: " << ffi->second->fileflow.tid);	    	
             proc->fileflows.erase(ffi);
	}
    }
    for(vector<FileFlowObj*>::iterator it = ffobjs.begin(); it != ffobjs.end(); it++) {
        (*it)->fileflow.endTs = endTs;	    
        (*it)->fileflow.opFlags |= OP_TRUNCATE;	    
        m_writer->writeFileFlow(&((*it)->fileflow));
        removeFileFlowFromSet(&(*it), true); 
    }
}


inline void FileFlowProcessor::updateFileFlow(FileFlowObj* ff, OpFlags flag, sinsp_evt* ev) {
       ff->fileflow.opFlags |= flag;
       sinsp_fdinfo_t * fdinfo = ev->get_fd_info();
       ff->lastUpdate = utils::getCurrentTime(m_cxt);
       if(flag == OP_OPEN) {
	   ff->fileflow.openFlags = fdinfo->m_openflags;
       }else if(flag == OP_WRITE_SEND) {
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

inline void FileFlowProcessor::processNewFlow(sinsp_evt* ev, /*sinsp_fdinfo_t * fdinfo,*/  ProcessObj* proc, FileObj* file, OpFlags flag, string flowkey) {
    FileFlowObj* ff = new FileFlowObj();
    ff->exportTime = utils::getExportTime(m_cxt); 
    ff->lastUpdate = utils::getCurrentTime(m_cxt);
    populateFileFlow(ff, flag, ev, proc, file, flowkey);
    updateFileFlow(ff, flag, ev);
    if(flag != OP_CLOSE) {
         proc->fileflows[flowkey] = ff;
         file->refs++;
        //m_fileflows[key] = nf;
        m_dfSet->insert(ff);
     }else {
        removeAndWriteRelatedFlows(proc, ff, ev->get_ts());	
        ff->fileflow.endTs = ev->get_ts();
        m_writer->writeFileFlow(&(ff->fileflow));
        delete ff;
     }
}

inline void FileFlowProcessor::removeAndWriteFileFlow(ProcessObj* proc, FileObj* file,  FileFlowObj** ff, string flowkey) {
    m_writer->writeFileFlow(&((*ff)->fileflow));
    //m_fileflows.erase(*key);
    //m_nfSet.erase((*nf));
    removeFileFlowFromSet(ff, false);
    removeFileFlow(proc, file, ff, flowkey);
}


inline void FileFlowProcessor::processExistingFlow(sinsp_evt* ev, ProcessObj* proc, FileObj* file, OpFlags flag, string flowkey, FileFlowObj* ff) {
      updateFileFlow(ff, flag, ev);
      if(flag == OP_CLOSE) {
          removeAndWriteRelatedFlows(proc, ff, ev->get_ts());	
          ff->fileflow.endTs = ev->get_ts();
          removeAndWriteFileFlow(proc, file, &ff, flowkey);
      }
}

int FileFlowProcessor::handleFileFlowEvent(sinsp_evt* ev, OpFlags flag) {
    sinsp_fdinfo_t * fdinfo =  ev->get_fd_info();


    if(fdinfo == NULL) {
       /*if(flag == OP_MMAP) {
           sinsp_threadinfo* ti = ev->get_thread_info();	       
           for(uint32_t i = 0; i < ev->get_num_params(); i ++) {
               string name = ev->get_param_name(i);
               const ppm_param_info* param = ev->get_param_info(i);
               const sinsp_evt_param* p = ev->get_param_value_raw(name.c_str());
               SF_DEBUG(m_logger, name  << " " << ev->get_param_value_str(name.c_str()) << " " <<  param->type << " " << (uint32_t)param->ninfo);
           }
	   const sinsp_evt_param* p = ev->get_param_value_raw("fd");
	   if(p != NULL) {
               int64_t fd = *(int64_t *)p->m_val;
	       if(fd > 0) {
	           fdinfo = ti->get_fd(fd);
		   if(fdinfo == NULL) {
                       SF_DEBUG(m_logger, "Event: " << ev->get_name() << " doesn't have an fdinfo associated with it! ");
                       return 1;

		   }
	           bool isfdnull = (fdinfo == NULL);
	           // string name = fdinfo->m_name;
		   string name = fdinfo->m_name;
	           SF_DEBUG(m_logger, " MMAP FD: " << fd << " " << isfdnull << " " << fdinfo->get_typechar() << " " << name );
	       }
	   }
       } else {*/
       SF_DEBUG(m_logger, "Event: " << ev->get_name() << " doesn't have an fdinfo associated with it! ");
       return 1;
       //}
    }

    if((fdinfo->is_ipv4_socket() || fdinfo->is_ipv6_socket())) {
        SF_WARN(m_logger, "handleFileFlowEvent cannot handle ip sockets,  Ignoring..") ;
        return 1;
    }
    char restype = fdinfo->get_typechar();

    switch(restype) {
        case SF_FILE:
        case SF_DIR:
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
    string filekey =  ti->m_container_id + fdinfo->m_name;
    //int i = ti->m_tid;
    //i = 1;
   // cout << (*(int64_t*)ti) << endl;
    //cout << "TID: " << ti->m_tid << " Container id " << ti->m_container_id << " FD name: " << fdinfo->m_name << endl;
    string flowkey = filekey  + std::to_string(ti->m_tid) + std::to_string(ev->get_fd_num());

    FileObj* file = m_fileCxt->getFile(ev, SFObjectState::REUP, created);

    FileFlowTable::iterator ffi = proc->fileflows.find(flowkey);
    if(ffi != proc->fileflows.end()) {
         ff = ffi->second;
    }
    SF_DEBUG(m_logger, proc->proc.exe << " Name: " <<  fdinfo->m_name << " type: " << fdinfo->get_typechar() <<  " " << file->file.path << " " <<  ev->get_name()); 

    if(ff == NULL) {
       processNewFlow(ev, proc, file, flag, flowkey);
    }else {
       processExistingFlow(ev, proc, file, flag, flowkey, ff);
    }
    return 0;
}

void FileFlowProcessor::removeFileFlow(ProcessObj* proc, FileObj* file,  FileFlowObj** ff, string flowkey) {
    proc->fileflows.erase(flowkey);
    delete *ff;
    ff = NULL;
    if(file != NULL) {
        file->refs--;
    }
}

int FileFlowProcessor::removeAndWriteFFFromProc(ProcessObj* proc, int64_t tid) {
    SF_DEBUG(m_logger, "CALLING removeAndWriteFFFromProc");
    int deleted = 0;
    for(FileFlowTable::iterator ffi = proc->fileflows.begin(); ffi != proc->fileflows.end(); ffi++) {
        if(tid == -1 || tid == ffi->second->fileflow.tid) {
            FileObj* file = m_fileCxt->getFile(ffi->second->filekey);
            ffi->second->fileflow.endTs = utils::getSysdigTime(m_cxt);
            if(tid != -1) {
		removeAndWriteRelatedFlows(proc, ffi->second, ffi->second->fileflow.endTs);
	    }
            ffi->second->fileflow.opFlags |= OP_TRUNCATE;
            SF_DEBUG(m_logger, "Writing FILEFLOW!!");
            m_writer->writeFileFlow(&(ffi->second->fileflow));
            FileFlowObj* ffo = ffi->second;
            proc->fileflows.erase(ffi);
            SF_DEBUG(m_logger, "Set size: " << m_dfSet->size());
            deleted += removeFileFlowFromSet(&ffo, true);
            SF_DEBUG(m_logger, "After Set size: " << m_dfSet->size());
            if(file == NULL) {
                SF_ERROR(m_logger, "File object doesn't exist for fileflow: " << ffi->second->filekey << ". This shouldn't happen.");
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
                     SF_DEBUG(m_logger, "Removing fileflow element from multiset"); 
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
           SF_ERROR(m_logger, "Cannot find FileFlow Object " << (*ffo)->filekey << " in data flow set. Deleting. This should not happen.");
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
     SF_DEBUG(m_logger, "Erasing flow!!! ");
     ProcessObj* proc = m_processCxt->getProcess(&(ffo->fileflow.procOID));
     if(proc == NULL) {
         SF_ERROR(m_logger, "Could not find proc " << ffo->fileflow.procOID.hpid << " " << ffo->fileflow.procOID.createTS << " This shouldn't happen!");
     } else {
          FileObj* file = m_fileCxt->getFile(ffo->filekey);
          if(file == NULL) {
             SF_ERROR(m_logger, "Unable to find file object of key " << ffo->filekey  << ". Shouldn't happen!!");
          }
          removeFileFlow(proc, file, &ffo, ffo->flowkey);
     }
}

void FileFlowProcessor::exportFileFlow(DataFlowObj* dfo, time_t now) {
     FileFlowObj* ffo = static_cast<FileFlowObj*>(dfo);
     ffo->fileflow.endTs = utils::getSysdigTime(m_cxt);
     m_processCxt->exportProcess(&(ffo->fileflow.procOID));
     m_fileCxt->exportFile(ffo->filekey);
     m_writer->writeFileFlow(&(ffo->fileflow));
     SF_DEBUG(m_logger, "Reupping flow!!! ");
     ffo->fileflow.ts = utils::getSysdigTime(m_cxt);
     ffo->fileflow.endTs = 0;
     ffo->fileflow.opFlags = 0;
     ffo->fileflow.numRRecvOps = 0;
     ffo->fileflow.numWSendOps = 0;
     ffo->fileflow.numRRecvBytes = 0;
     ffo->fileflow.numWSendBytes = 0;
}

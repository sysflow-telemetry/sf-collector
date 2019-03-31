#include "networkflowprocessor.h"
#include "utils.h"
using namespace networkflow;


FileFlowProcessor::FileFlowProcessor(SysFlowContext* cxt, SysFlowWriter* writer, process::ProcessContext* processCxt, DataFlowSet* dfSet)  {
    m_cxt = cxt;
    m_writer = writer;
    m_processCxt = processCxt;
    m_dfSet = dfSet;
}

FileFlowProcessor::~FileFlowProcessor() {
}

inline int32_t FileFlowProcessor::getProtocol(scap_l4_proto proto) {
    int32_t prt = -1;
    switch(proto)
    {
        case SCAP_L4_TCP:
            prt = 6;
            break;
        case SCAP_L4_UDP:
            prt = 17;
            break;
        case SCAP_L4_ICMP:
            prt = 1;
            break;
        case SCAP_L4_RAW:
            prt = 254;
            break;
         default:
             break;
    }
    return prt;
}

inline void FileFlowProcessor::populateFileFlow(FileFlowObj* ff, OpFlags flag, sinsp_evt* ev, ProcessObj* proc, FileObj* file) {
   sinsp_fdinfo_t * fdinfo = ev->get_fd_info();
   sinsp_threadinfo* ti = ev->get_thread_info();
   ff->fileflow.opFlags = flag;
   ff->fileflow.ts = ev->get_ts();
   ff->fileflow.endTs = 0;
   ff->fileflow.procOID.hpid = proc->proc.oid.hpid;
   ff->fileflow.procOID.createTS = proc->proc.oid.createTS;
   ff->fileflow.tid = ti->m_tid;
   ff->fileflow.fd = ev->get_fd_num();
   ff->fileflow.fileOID = file->file.oid;
   ff->fileflow.numROps = 0;
   ff->fileflow.numWOps = 0;
   ff->fileflow.numRBytes = 0;
   ff->fileflow.numWBytes = 0;
}

inline void FileFlowProcessor::updateFileFlow(FileFlowObj* ff, OpFlags flag, sinsp_evt* ev) {
       ff->fileflow.opFlags |= flag;
       ff->lastUpdate = utils::getCurrentTime(m_cxt);
       if(flag == OP_WRITE_SEND) {
           ff->fileflow.numWOps++;
           int res = utils::getSyscallResult(ev);
           if(res > 0 ) {
               ff->fileflow.numWBytes+= res;
           }
       }else if(flag == OP_READ_RECV) {
           ff->fileflow.numROps++;
           int res = utils::getSyscallResult(ev);
           if(res > 0 ) {
               ff->fileflow.numRBytes+= res;
           }
       }
}

inline void FileFlowProcessor::processNewFlow(sinsp_evt* ev, ProcessObj* proc, OpFlags flag, NFKey key) {
    NetFlowObj* nf = new NetFlowObj();
    nf->exportTime = utils::getExportTime(m_cxt); 
    nf->lastUpdate = utils::getCurrentTime(m_cxt);
    populateNetFlow(nf, flag, ev, proc);
    updateNetFlow(nf, flag, ev);
    if(flag != OP_CLOSE) {
         proc->fileflows[key] = nf;
        //m_fileflows[key] = nf;
        m_dfSet->insert(nf);
     }else {
        nf->fileflow.endTs = ev->get_ts();
        m_writer->writeNetFlow(&(nf->fileflow));
        delete nf;
     }
}

inline void FileFlowProcessor::removeAndWriteNetworkFlow(ProcessObj* proc, NetFlowObj** nf, NFKey* key) {
    m_writer->writeNetFlow(&((*nf)->fileflow));
    //m_fileflows.erase(*key);
    //m_nfSet.erase((*nf));
    removeNetworkFlowFromSet(nf, false);
    removeNetworkFlow(proc, nf, key);
}


inline void FileFlowProcessor::processExistingFlow(sinsp_evt* ev, ProcessObj* proc, OpFlags flag, NFKey key, NetFlowObj* nf) {
     //sinsp_fdinfo_t * fdinfo = ev->get_fd_info();
     /*
     if(!(nf->fileflow.procOID.hpid == proc->oid.hpid  &&
          nf->fileflow.procOID.createTS == proc->oid.createTS)){
          //DO WE WANT TO ALLOW processes that aren't ancestors of each other to delegate? If not,
          //what should we do?
          if(!m_processCxt->isAncestor(&(nf->fileflow.procOID), proc)) {
               cout << "Netflow: " << nf->fileflow.sip << " " << nf->fileflow.dip << " " << nf->fileflow.sport << " " << nf->fileflow.dport << " already exists but isn't an ancestor of: " << proc->exe <<  " Writing! " << endl;
               Process* nfProc = m_processCxt->getProcess(&(nf->fileflow.procOID));
               if(nfProc != NULL) {
                   cout << "Netflow proc oid: " << nf->fileflow.procOID.hpid << " " << nf->fileflow.procOID.createTS << " " << nfProc->exe << " " << nfProc->exeArgs << "Netflow proc ancestors: Old proc" <<  endl;
                   m_processCxt->printAncestors(nfProc);
                   cout << "Current proc oid: " << proc->oid.hpid << " " << proc->oid.createTS << " " << proc->exe << " " << proc->exeArgs << " Current Process ancestors: Is ROLE SERVER? " << fdinfo->is_role_server() << endl;
                   m_processCxt->printAncestors(proc);
              }else {
                   cout << "Netflow proc oid: " << nf->fileflow.procOID.hpid << " " << nf->fileflow.procOID.createTS << " " << "Process is NULL" << endl;
              }
          } 
          nf->fileflow.opFlags |= OP_NF_DELEGATE;
          m_writer->writeNetFlow(&(nf->fileflow));
          nf->fileflow.opFlags &= ~OP_NF_DELEGATE;
          nf->fileflow.opFlags |= OP_NF_INHERIT;
          nf->fileflow.procOID.createTS = proc->oid.createTS;
          nf->fileflow.procOID.hpid = proc->oid.hpid;
      }*/
      updateNetFlow(nf, flag, ev);
      if(flag == OP_CLOSE) {
          nf->fileflow.endTs = ev->get_ts();
          removeAndWriteNetworkFlow(proc, &nf, &key);
      }
}
int FileFlowProcessor::handleNetFlowEvent(sinsp_evt* ev, OpFlags flag) {
    sinsp_fdinfo_t * fdinfo = ev->get_fd_info();
    if(fdinfo == NULL) {
       cout << "Uh oh!!! Event: " << ev->get_name() << " doesn't have an fdinfo associated with it! " << endl;
       return 1;
    }

    if(!(fdinfo->is_ipv4_socket() || fdinfo->is_ipv6_socket())) {
        cout << "handleNetFlowEvent can only handle ip sockets, not file descriptor of type: " << fdinfo->get_typechar() << ". Ignoring.." << endl;
        return 1;
    }

    if(fdinfo->is_ipv6_socket()) {
        cout << "IPv6 is not supported in the current version of SysFlow.  Ignoring.." << endl;
        return 1;
    }
    bool created = false;
    //calling get process is important because it ensures that the process object has been written to the
    //sysflow file.   This is important for long running NetworkFlows that may span across files.
    ProcessObj* proc = m_processCxt->getProcess(ev, SFObjectState::REUP, created);
    NetFlowObj* nf = NULL;
    NFKey key;
    canonicalizeKey(fdinfo, &key);
    NetworkFlowTable::iterator nfi = proc->fileflows.find(key);
    if(nfi != proc->fileflows.end()) {
         nf = nfi->second;
    }

    string ip4tuple = ipv4tuple_to_string(&(fdinfo->m_sockinfo.m_ipv4info), false);

    cout << proc->proc.exe << " " << ip4tuple << " Proto: " << getProtocol(fdinfo->get_l4proto()) << " Server: " << fdinfo->is_role_server() << " Client: " << fdinfo->is_role_client() << " " << ev->get_name() <<  endl;

    if(nf == NULL) {
       processNewFlow(ev, proc, flag, key);
    }else {
       processExistingFlow(ev, proc, flag, key, nf);
    }
    return 0;
}

void FileFlowProcessor::removeNetworkFlow(ProcessObj* proc, NetFlowObj** nf, NFKey* key) {
    proc->fileflows.erase(*key);
    delete *nf;
    nf = NULL;
}

int FileFlowProcessor::removeAndWriteNFFromProc(ProcessObj* proc) {
    cout << "CALLING removeANDWRITE" << endl;
    int deleted = 0;
    for(NetworkFlowTable::iterator nfi = proc->fileflows.begin(); nfi != proc->fileflows.end(); nfi++) {
        nfi->second->fileflow.endTs = utils::getSysdigTime(m_cxt);
        nfi->second->fileflow.opFlags |= OP_TRUNCATE;
        cout << "Writing NETFLOW!!" << endl;
        m_writer->writeNetFlow(&(nfi->second->fileflow));
        NetFlowObj* nfo = nfi->second;
        cout << "Set size: " << m_dfSet->size() << endl;
        deleted += removeNetworkFlowFromSet(&nfo, true);
        cout << "After Set size: " << m_dfSet->size() << endl;
    }
    proc->fileflows.clear();
    return deleted;
}

int FileFlowProcessor::removeNetworkFlowFromSet(NetFlowObj** nfo, bool deleteNetFlow) {
        bool found = false;
        int removed = 0;
        for(DataFlowSet::iterator iter = m_dfSet->find(*nfo); iter != m_dfSet->end(); iter++) {
            if((*iter)->isNetworkFlow) {
                NetFlowObj* foundObj = static_cast<NetFlowObj*>(*iter);
                //cout << "Found: " << foundObj->fileflow.procOID.createTS << " " << foundObj->fileflow.procOID.hpid << endl;
                if(*foundObj == **nfo) {
                     cout << "Removing element from multiset" << endl;
                     m_dfSet->erase(iter);
                     if(deleteNetFlow) {
                        delete *nfo;
                        nfo = NULL;
                     }
                     removed++;
                     found = true;
                     break;   
               }
           }
        }
        if(!found) {
           cout << "Error: Cannot find Netflow Object in data flow set. Deleting. This should not happen." << endl;
           if(deleteNetFlow) {
               delete *nfo;
               nfo = NULL;
           }
        }
        return removed;
}

void FileFlowProcessor::removeNetworkFlow(DataFlowObj* dfo) {
     NFKey key;
     NetFlowObj* nfo = static_cast<NetFlowObj*>(dfo);
     //do we want to write out a fileflow that hasn't had any action in an interval?
     //nfo->fileflow.endTs = utils::getSysdigTime(m_cxt);
     //m_writer->writeNetFlow(&(nfo->fileflow));
     canonicalizeKey(nfo, &key);
     cout << "Erasing flow!!! " << endl;
     ProcessObj* proc = m_processCxt->getProcess(&(nfo->fileflow.procOID));
     if(proc == NULL) {
         cout << "Error: Could not find proc " << nfo->fileflow.procOID.hpid << " " << nfo->fileflow.procOID.createTS << " This shouldn't happen!" << endl;
     } else {
          removeNetworkFlow(proc, &nfo, &key);
     }
}

void FileFlowProcessor::exportNetworkFlow(DataFlowObj* dfo, time_t now) {
     NetFlowObj* nfo = static_cast<NetFlowObj*>(dfo);
     nfo->fileflow.endTs = utils::getSysdigTime(m_cxt);
     m_processCxt->exportProcess(&(nfo->fileflow.procOID));
     m_writer->writeNetFlow(&(nfo->fileflow));
     cout << "Reupping flow!!! " << endl;
     nfo->fileflow.ts = utils::getSysdigTime(m_cxt);
     nfo->fileflow.endTs = 0;
     nfo->fileflow.opFlags = 0;
     nfo->fileflow.numROps = 0;
     nfo->fileflow.numWOps = 0;
     nfo->fileflow.numRBytes = 0;
     nfo->fileflow.numWBytes = 0;
}

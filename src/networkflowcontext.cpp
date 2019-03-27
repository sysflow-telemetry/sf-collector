#include "networkflowcontext.h"
#include "utils.h"
using namespace networkflow;


NetworkFlowContext::NetworkFlowContext(SysFlowContext* cxt, SysFlowWriter* writer, process::ProcessContext* processCxt) : m_nfSet(), m_oidnfTable(PROC_TABLE_SIZE) {
    m_nfdelkey.ip1 = 1;
    m_nfdelkey.ip2 = 1;
    m_nfdelkey.port1 = 1;
    m_nfdelkey.port2 = 1;
    m_nfemptykey.ip1 = 1;
    m_nfemptykey.ip2 = 0;
    m_nfemptykey.port1 = 1;
    m_nfemptykey.port2 = 1;
 //   m_netflows.set_empty_key(m_nfemptykey);
  //  m_netflows.set_deleted_key(m_nfdelkey);
    m_oidemptykey.hpid = 2; 
    m_oidemptykey.createTS = 2; 
    m_oiddelkey.hpid = 1;
    m_oiddelkey.createTS = 1;
    m_oidnfTable.set_empty_key(m_oidemptykey);
    m_oidnfTable.set_deleted_key(m_oiddelkey);
   

    m_cxt = cxt;
    m_writer = writer;
    m_processCxt = processCxt;
    m_lastCheck = 0;
}

NetworkFlowContext::~NetworkFlowContext() {
    clearNetFlows();
}

NetworkFlowTable* NetworkFlowContext::createNetworkFlowTable() {
    NetworkFlowTable* nft = new NetworkFlowTable();
    nft->set_empty_key(m_nfemptykey);
    nft->set_deleted_key(m_nfdelkey);
    return nft;
}




inline int32_t NetworkFlowContext::getProtocol(scap_l4_proto proto) {
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


inline void NetworkFlowContext::canonicalizeKey(sinsp_fdinfo_t* fdinfo, NFKey* key) {
    uint32_t sip = fdinfo->m_sockinfo.m_ipv4info.m_fields.m_sip;
    uint32_t dip = fdinfo->m_sockinfo.m_ipv4info.m_fields.m_dip;
    uint32_t sport = fdinfo->m_sockinfo.m_ipv4info.m_fields.m_sport;
    uint32_t dport = fdinfo->m_sockinfo.m_ipv4info.m_fields.m_dport;

    //key->oid.hpid = oid->hpid;
    //key->oid.createTS = oid->createTS;

//    if(sip < dip) {
       key->ip1 = sip;
       key->port1 = sport;
       key->ip2 = dip;
       key->port2 = dport;
 /*   } else if(dip < sip) {
       key->ip1 = dip;
       key->port1 = dport;
       key->ip2 = sip;
       key->port2 = sport;
    } else if(sport < dport) {
       key->ip1 = sip;
       key->port1 = sport;
       key->ip2 = dip;
       key->port2 = dport;
    } else {
       key->ip1 = dip;
       key->port1 = dport;
       key->ip2 = sip;
       key->port2 = sport;
    }*/
}

inline void NetworkFlowContext::canonicalizeKey(NetFlowObj* nf, NFKey* key) {
    uint32_t sip = nf->netflow.sip;
    uint32_t dip = nf->netflow.dip;
    uint32_t sport = nf->netflow.sport;
    uint32_t dport = nf->netflow.dport;
    //key->oid.hpid = oid->hpid;
    //key->oid.createTS = oid->createTS;

   // if(sip < dip) {
       key->ip1 = sip;
       key->port1 = sport;
       key->ip2 = dip;
       key->port2 = dport;
   /* } else if(dip < sip) {
       key->ip1 = dip;
       key->port1 = dport;
       key->ip2 = sip;
       key->port2 = sport;
    } else if(sport < dport) {
       key->ip1 = sip;
       key->port1 = sport;
       key->ip2 = dip;
       key->port2 = dport;
    } else {
       key->ip1 = dip;
       key->port1 = dport;
       key->ip2 = sip;
       key->port2 = sport;
    }*/
}

inline time_t NetworkFlowContext::getExportTime() {
    time_t now = utils::getCurrentTime(m_cxt);
    struct tm exportTM = *localtime( &now);
    exportTM.tm_sec += m_cxt->getNFExportInterval();   // add 30 seconds to the time
    return mktime( &exportTM);      // normalize iti
}

inline void NetworkFlowContext::populateNetFlow(NetFlowObj* nf, NFOpFlags flag, sinsp_evt* ev, Process* proc) {
   sinsp_fdinfo_t * fdinfo = ev->get_fd_info();
   sinsp_threadinfo* ti = ev->get_thread_info();
   nf->netflow.opFlags = flag;
   nf->netflow.ts = ev->get_ts();
   nf->netflow.endTs = 0;
   nf->netflow.procOID.hpid = proc->oid.hpid;
   nf->netflow.procOID.createTS = proc->oid.createTS;
   nf->netflow.tid = ti->m_tid;
   nf->netflow.sip = fdinfo->m_sockinfo.m_ipv4info.m_fields.m_sip; 
   nf->netflow.dip = fdinfo->m_sockinfo.m_ipv4info.m_fields.m_dip; 
   nf->netflow.sport = fdinfo->m_sockinfo.m_ipv4info.m_fields.m_sport; 
   nf->netflow.dport = fdinfo->m_sockinfo.m_ipv4info.m_fields.m_dport; 
   nf->netflow.proto = getProtocol(fdinfo->get_l4proto());
   nf->netflow.numROps = 0;
   nf->netflow.numWOps = 0;
   nf->netflow.numRBytes = 0;
   nf->netflow.numWBytes = 0;
}

inline void NetworkFlowContext::updateNetFlow(NetFlowObj* nf, NFOpFlags flag, sinsp_evt* ev) {
       nf->netflow.opFlags |= flag;
       nf->lastUpdate = utils::getCurrentTime(m_cxt);
       if(flag == OP_NF_SEND) {
           nf->netflow.numWOps++;
           int res = utils::getSyscallResult(ev);
           if(res > 0 ) {
               nf->netflow.numWBytes+= res;
           }
       }else if(flag == OP_NF_RECV) {
           nf->netflow.numROps++;
           int res = utils::getSyscallResult(ev);
           if(res > 0 ) {
               nf->netflow.numRBytes+= res;
           }
       }
}

inline void NetworkFlowContext::processNewFlow(sinsp_evt* ev, Process* proc, NFOpFlags flag, NFKey key) {
    NetFlowObj* nf = new NetFlowObj();
    nf->exportTime = getExportTime(); 
    nf->lastUpdate = utils::getCurrentTime(m_cxt);
    populateNetFlow(nf, flag, ev, proc);
    updateNetFlow(nf, flag, ev);
    if(flag != OP_NF_CLOSE) {
        OIDNetworkTable::iterator it = m_oidnfTable.find(proc->oid);
        if(it != m_oidnfTable.end()) {
            (*(it->second))[key] = nf;
        }else {
           NetworkFlowTable* nft = createNetworkFlowTable();
           (*nft)[key] = nf;
           m_oidnfTable[proc->oid] = nft;
        }
        //m_netflows[key] = nf;
        m_nfSet.insert(nf);
     }else {
        nf->netflow.endTs = ev->get_ts();
        m_writer->writeNetFlow(&(nf->netflow));
        delete nf;
     }
}

inline void NetworkFlowContext::removeAndWriteNetworkFlow(NetFlowObj** nf, NFKey* key) {
    m_writer->writeNetFlow(&((*nf)->netflow));
    //m_netflows.erase(*key);
    m_nfSet.erase((*nf));
    removeNetworkFlow(nf, key);
}


inline void NetworkFlowContext::processExistingFlow(sinsp_evt* ev, Process* proc, NFOpFlags flag, NFKey key, NetFlowObj* nf) {
     //sinsp_fdinfo_t * fdinfo = ev->get_fd_info();
     /*
     if(!(nf->netflow.procOID.hpid == proc->oid.hpid  &&
          nf->netflow.procOID.createTS == proc->oid.createTS)){
          //DO WE WANT TO ALLOW processes that aren't ancestors of each other to delegate? If not,
          //what should we do?
          if(!m_processCxt->isAncestor(&(nf->netflow.procOID), proc)) {
               cout << "Netflow: " << nf->netflow.sip << " " << nf->netflow.dip << " " << nf->netflow.sport << " " << nf->netflow.dport << " already exists but isn't an ancestor of: " << proc->exe <<  " Writing! " << endl;
               Process* nfProc = m_processCxt->getProcess(&(nf->netflow.procOID));
               if(nfProc != NULL) {
                   cout << "Netflow proc oid: " << nf->netflow.procOID.hpid << " " << nf->netflow.procOID.createTS << " " << nfProc->exe << " " << nfProc->exeArgs << "Netflow proc ancestors: Old proc" <<  endl;
                   m_processCxt->printAncestors(nfProc);
                   cout << "Current proc oid: " << proc->oid.hpid << " " << proc->oid.createTS << " " << proc->exe << " " << proc->exeArgs << " Current Process ancestors: Is ROLE SERVER? " << fdinfo->is_role_server() << endl;
                   m_processCxt->printAncestors(proc);
              }else {
                   cout << "Netflow proc oid: " << nf->netflow.procOID.hpid << " " << nf->netflow.procOID.createTS << " " << "Process is NULL" << endl;
              }
          } 
          nf->netflow.opFlags |= OP_NF_DELEGATE;
          m_writer->writeNetFlow(&(nf->netflow));
          nf->netflow.opFlags &= ~OP_NF_DELEGATE;
          nf->netflow.opFlags |= OP_NF_INHERIT;
          nf->netflow.procOID.createTS = proc->oid.createTS;
          nf->netflow.procOID.hpid = proc->oid.hpid;
      }*/
      updateNetFlow(nf, flag, ev);
      if(flag == OP_NF_CLOSE) {
          nf->netflow.endTs = ev->get_ts();
          removeAndWriteNetworkFlow(&nf, &key);
      }
}
int NetworkFlowContext::handleNetFlowEvent(sinsp_evt* ev, NFOpFlags flag) {
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
    Process* proc = m_processCxt->getProcess(ev, ActionType::REUP, created);
    NetFlowObj* nf = NULL;
    NFKey key;
    canonicalizeKey(fdinfo, &key);
    OIDNetworkTable::iterator it = m_oidnfTable.find(proc->oid);
    if(it != m_oidnfTable.end()) {
        NetworkFlowTable::iterator nfi = it->second->find(key);
        if(nfi != it->second->end()) {
           nf = nfi->second;
        }
    }

    string ip4tuple = ipv4tuple_to_string(&(fdinfo->m_sockinfo.m_ipv4info), false);

    cout << proc->exe << " " << ip4tuple << " Proto: " << getProtocol(fdinfo->get_l4proto()) << " Server: " << fdinfo->is_role_server() << " Client: " << fdinfo->is_role_client() << " " << ev->get_name() <<  endl;

    if(nf == NULL) {
       processNewFlow(ev, proc, flag, key);
    }else {
       processExistingFlow(ev, proc, flag, key, nf);
    }
    return 0;
}

void NetworkFlowContext::removeNetworkFlow(NetFlowObj** nf, NFKey* key) {
    OIDNetworkTable::iterator it = m_oidnfTable.find((*nf)->netflow.procOID);
    if(it != m_oidnfTable.end()) {
       it->second->erase(*key);
       if(it->second->empty()) {
           delete it->second;
           m_oidnfTable.erase(it);
       }
    }
    delete *nf;
    nf = NULL;
}

int NetworkFlowContext::removeAndWriteNFFromProc(OID* oid) {
    cout << "CALLING removeANDWRITE" << endl;
    OIDNetworkTable::iterator it = m_oidnfTable.find(*oid);
    int deleted = 0;
    if(it != m_oidnfTable.end()) {
       for(NetworkFlowTable::iterator nfi = it->second->begin(); nfi != it->second->end(); nfi++) {
             nfi->second->netflow.endTs = utils::getSysdigTime(m_cxt);
             nfi->second->netflow.opFlags |= OP_NF_TRUNCATE;
             cout << "Writing NETFLOW!!" << endl;
             m_writer->writeNetFlow(&(nfi->second->netflow));
             NetFlowObj* nfo = nfi->second;
             cout << "Set size: " << m_nfSet.size() << endl;
             for(NetworkFlowSet::iterator iter = m_nfSet.find(nfo); iter != m_nfSet.end(); iter++) {
                NetFlowObj* foundObj = (*iter);
                //cout << "Found: " << foundObj->netflow.procOID.createTS << " " << foundObj->netflow.procOID.hpid << endl;
                if(*foundObj == *nfo) {
                     cout << "Removing element from multiset" << endl;
                     m_nfSet.erase(iter);
                     delete nfo;
                     deleted++;
                     break;   
                 } 
             }
             //m_nfSet.erase(nfo);
             cout << "After Set size: " << m_nfSet.size() << endl;
             //delete nfo;
             //delete nfi->second;
       }
       delete it->second;
       m_oidnfTable.erase(it);
    }
    return deleted;
}

int NetworkFlowContext::checkForExpiredFlows() {
     time_t now = utils::getCurrentTime(m_cxt);
     if(m_lastCheck == 0) {
        m_lastCheck = now;
        return 0;
     }
     if(difftime(now, m_lastCheck) < 1.0) {
        return 0;
     }
     m_lastCheck = now;
     NFKey key;
     int i = 0;
     cout << "Checking expired Flows!!!...." << endl;
     for(NetworkFlowSet::iterator it = m_nfSet.begin(); it != m_nfSet.end(); ) {
             cout << "Checking flow with exportTime: " << (*it)->exportTime << " Now: " << now << endl;
            if((*it)->exportTime <= now) {
                 cout << "Exporting flow!!! " << endl; 
                (*it)->netflow.endTs = utils::getSysdigTime(m_cxt);
                m_writer->writeNetFlow(&((*it)->netflow));
                if(difftime(now, (*it)->lastUpdate) >= m_cxt->getNFExpireInterval()) {
                      canonicalizeKey((*it), &key);
                      //m_netflows.erase(key);
                      //delete (*it);
                       cout << "Erasing flow!!! " << endl;
                       NetFlowObj* nfObj = (*it);
                       removeNetworkFlow(&nfObj, &key);
                      it = m_nfSet.erase(it);
                } else {
                     //else reup the expire time...remove from the m_nfSet and put back in.
                     cout << "Reupping flow!!! " << endl;
                     NetFlowObj* nf = (*it);
                     nf->netflow.ts = utils::getSysdigTime(m_cxt);
                     nf->netflow.endTs = 0;
                     nf->netflow.opFlags = 0;
                     nf->netflow.numROps = 0;
                     nf->netflow.numWOps = 0;
                     nf->netflow.numRBytes = 0;
                     nf->netflow.numWBytes = 0;
                     it = m_nfSet.erase(it);
                     nf->exportTime = getExportTime();
                     m_nfSet.insert(nf);
                }
                i++;
            }else {
               break;
            }
     }
     return i;
}


void NetworkFlowContext::clearNetFlows() {
     for(OIDNetworkTable::iterator it = m_oidnfTable.begin(); it != m_oidnfTable.end(); it++) {
         for(NetworkFlowTable::iterator nfi = it->second->begin(); nfi != it->second->end(); nfi++) {
             nfi->second->netflow.opFlags |= OP_NF_TRUNCATE;
             nfi->second->netflow.endTs = utils::getSysdigTime(m_cxt);
             m_writer->writeNetFlow(&(nfi->second->netflow));
             delete nfi->second;
          }
          it->second->clear();
          delete it->second;
     }
     m_oidnfTable.clear();
     m_nfSet.clear();
}

#include "networkflowcontext.h"
#include "utils.h"
using namespace networkflow;


NetworkFlowContext::NetworkFlowContext(SysFlowContext* cxt, SysFlowWriter* writer, process::ProcessContext* processCxt) : m_netflows(NF_TABLE_SIZE), m_nfSet() {
    m_nfdelkey.ip1 = 0;
    m_nfdelkey.ip2 = 0;
    m_nfdelkey.port1 = 0;
    m_nfdelkey.port2 = 0;
    m_nfemptykey.ip1 = 0;
    m_nfemptykey.ip2 = 0;
    m_nfemptykey.port1 = 0;
    m_nfemptykey.port2 = 1;
    m_netflows.set_empty_key(m_nfemptykey);
    m_netflows.set_deleted_key(m_nfdelkey);
    m_cxt = cxt;
    m_writer = writer;
    m_processCxt = processCxt;
    m_lastCheck = 0;
}

NetworkFlowContext::~NetworkFlowContext() {
    clearNetFlows();
}


inline void NetworkFlowContext::canonicalizeKey(sinsp_fdinfo_t* fdinfo, NFKey* key) {
    uint32_t sip = fdinfo->m_sockinfo.m_ipv4info.m_fields.m_sip;
    uint32_t dip = fdinfo->m_sockinfo.m_ipv4info.m_fields.m_dip;
    uint32_t sport = fdinfo->m_sockinfo.m_ipv4info.m_fields.m_sport;
    uint32_t dport = fdinfo->m_sockinfo.m_ipv4info.m_fields.m_dport;

    if(sip < dip) {
       key->ip1 = sip;
       key->port1 = sport;
       key->ip2 = dip;
       key->port2 = dport;
    } else if(dip < sip) {
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
    }
}

inline void NetworkFlowContext::canonicalizeKey(NetFlowObj* nf, NFKey* key) {
    uint32_t sip = nf->netflow.sip;
    uint32_t dip = nf->netflow.dip;
    uint32_t sport = nf->netflow.sport;
    uint32_t dport = nf->netflow.dport;

    if(sip < dip) {
       key->ip1 = sip;
       key->port1 = sport;
       key->ip2 = dip;
       key->port2 = dport;
    } else if(dip < sip) {
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
    }
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
   nf->netflow.startTs = ev->get_ts();
   nf->netflow.endTs = 0;
   nf->netflow.procOID.hpid = proc->oid.hpid;
   nf->netflow.procOID.createTS = proc->oid.createTS;
   nf->netflow.tid = ti->m_tid;
   nf->netflow.sip = fdinfo->m_sockinfo.m_ipv4info.m_fields.m_sip; 
   nf->netflow.dip = fdinfo->m_sockinfo.m_ipv4info.m_fields.m_dip; 
   nf->netflow.sport = fdinfo->m_sockinfo.m_ipv4info.m_fields.m_sport; 
   nf->netflow.dport = fdinfo->m_sockinfo.m_ipv4info.m_fields.m_dport; 
   nf->netflow.proto = fdinfo->get_l4proto();
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
        m_netflows[key] = nf;
        m_nfSet.insert(nf);
     }else {
        nf->netflow.endTs = ev->get_ts();
        m_writer->writeNetFlow(&(nf->netflow));
        delete nf;
     }
}

inline void NetworkFlowContext::removeAndWriteNetworkFlow(NetFlowObj** nf, NFKey* key) {
    m_writer->writeNetFlow(&((*nf)->netflow));
    m_netflows.erase(*key);
    m_nfSet.erase((*nf));
    delete *nf;
    nf = NULL;
}


inline void NetworkFlowContext::processExistingFlow(sinsp_evt* ev, Process* proc, NFOpFlags flag, NFKey key, NetFlowObj* nf) {
     if(!(nf->netflow.procOID.hpid == proc->oid.hpid  &&
          nf->netflow.procOID.createTS == proc->oid.createTS)){
          //DO WE WANT TO ALLOW processes that aren't ancestors of each other to delegate? If not,
          //what should we do?
          if(!m_processCxt->isAncestor(&(nf->netflow.procOID), proc)) {
               cout << "Netflow: " << nf->netflow.sip << " " << nf->netflow.dip << " " << nf->netflow.sport << " " << nf->netflow.dport << " already exists but isn't an ancestor of: " << proc->exe <<  " Writing! " << endl;
          } 
          nf->netflow.opFlags |= OP_NF_DELEGATE;
          m_writer->writeNetFlow(&(nf->netflow));
          nf->netflow.opFlags &= ~OP_NF_DELEGATE;
          nf->netflow.opFlags |= OP_NF_INHERIT;
          nf->netflow.procOID.createTS = proc->oid.createTS;
          nf->netflow.procOID.hpid = proc->oid.hpid;
          nf->lastUpdate = utils::getCurrentTime(m_cxt);

      }
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
    NetworkFlowTable::iterator it = m_netflows.find(key);
    if(it != m_netflows.end()) {
        nf = it->second;
    }


    if(nf == NULL) {
       processNewFlow(ev, proc, flag, key);
    }else {
       processExistingFlow(ev, proc, flag, key, nf);
    }
    return 0;
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
     for(NetworkFlowSet::iterator it = m_nfSet.begin(); it != m_nfSet.end(); ) {
            if((*it)->exportTime <= now) {
                (*it)->netflow.endTs = utils::getSysdigTime(m_cxt);
                m_writer->writeNetFlow(&((*it)->netflow));
                if(difftime(now, (*it)->lastUpdate) >= m_cxt->getNFExpireInterval()) {
                      canonicalizeKey((*it), &key);
                      m_netflows.erase(key);
                      delete (*it);
                      it = m_nfSet.erase(it);
                } else {
                     //else reup the expire time...remove from the m_nfSet and put back in.
                     it++;
                }

            }else {
               break;
            }
     }



     return i;
}


void NetworkFlowContext::clearNetFlows() {

}

#include "networkflow.h"
#include "context.h"
#include "process.h"
#include "hashtables.h"
#include "utils.h"
using namespace networkflow;


inline void canonicalizeKey(sinsp_fdinfo_t* fdinfo, networkflow::NFKey* key) {
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

inline time_t getExpireTime() {
    time_t now = time( NULL);
    struct tm expireTM = *localtime( &now);
    expireTM.tm_sec += 30;   // add 30 seconds to the time

    return mktime( &expireTM);      // normalize iti
}

inline void populateNetFlow(NetFlowObj* nf, NFOpFlags flag, sinsp_evt* ev, Process* proc) {
   sinsp_fdinfo_t * fdinfo = ev->get_fd_info();
   sinsp_threadinfo* ti = ev->get_thread_info();
   nf->netflow.opFlags = flag;
   nf->netflow.startTs = ev->get_ts();
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


int networkflow::handleNetFlowEvent(Context* cxt, sinsp_evt* ev, NFOpFlags flag) {
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
    Process* proc = process::getProcess(cxt, ev, ActionType::REUP, created);
    NetFlowObj* nf = NULL;
    NFKey key;
    canonicalizeKey(fdinfo, &key);
    NetworkFlowTable::iterator it = cxt->netflows.find(key);
    if(it != cxt->netflows.end()) {
        nf = it->second;
    }


    if(nf == NULL) {
       nf = new NetFlowObj();
       nf->lastUpdate = time(NULL);
       nf->expireTime = getExpireTime(); 
       populateNetFlow(nf, flag, ev, proc);
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
       if(flag != OP_NF_CLOSE) {
          cxt->netflows[key] = nf;
       }else {
          networkflow::writeNetFlow(cxt, nf);
          delete nf;
      }


   }




/*
    if(flag == OP_NF_ACCEPT || flag == OP_NF_CONNECT)  {

    } else if (flag == OP_NF_SEND || flag OP_NF_RECV) {

    } else if (flag == OP_NF_CLOSE) {

    }
*/ 


    

    return 0;
}

void networkflow::writeNetFlow(Context* cxt, NetFlowObj* nf) {
   cxt->flow.rec.set_NetworkFlow(nf->netflow);
   cxt->numRecs++;
   cxt->dfw->write(cxt->flow);
}

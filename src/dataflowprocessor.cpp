#include "dataflowprocessor.h"

using namespace dataflow;

DataFlowProcessor::DataFlowProcessor(SysFlowContext* cxt, SysFlowWriter* writer, process::ProcessContext* processCxt) : m_dfSet() {
    m_cxt = cxt;
    m_netflowPrcr = new networkflow::NetworkFlowProcessor(cxt, writer, processCxt, &m_dfSet);
    m_lastCheck = 0;
}

DataFlowProcessor::~DataFlowProcessor() {
   //clearTables();
    if(m_netflowPrcr != NULL) {
        delete m_netflowPrcr;
    }
}


int DataFlowProcessor::handleDataEvent(sinsp_evt* ev, OpFlags flag) {
    sinsp_fdinfo_t * fdinfo = ev->get_fd_info();
    if(fdinfo == NULL && ev->get_type() == PPME_SYSCALL_SELECT_X) {
       return 0;
    }

    if(fdinfo == NULL) {
       cout << "Uh oh!!! Event: " << ev->get_name() << " doesn't have an fdinfo associated with it! ErrorCode: " << utils::getSyscallResult(ev) << endl;
       return 1;
    }
    if(fdinfo->is_ipv4_socket() || fdinfo->is_ipv6_socket()) {
      return m_netflowPrcr->handleNetFlowEvent(ev, flag);
    }
    return 2;
}

int DataFlowProcessor::removeAndWriteDFFromProc(ProcessObj* proc) {
   return m_netflowPrcr->removeAndWriteNFFromProc(proc);
}


int DataFlowProcessor::checkForExpiredRecords() {
     time_t now = utils::getCurrentTime(m_cxt);
     if(m_lastCheck == 0) {
        m_lastCheck = now;
        return 0;
     }
     if(difftime(now, m_lastCheck) < 1.0) {
        return 0;
     }
     m_lastCheck = now;
     int i = 0;
     cout << "Checking expired Flows!!!...." << endl;
     for(DataFlowSet::iterator it = m_dfSet.begin(); it != m_dfSet.end(); ) {
             cout << "Checking flow with exportTime: " << (*it)->exportTime << " Now: " << now << endl;
            if((*it)->exportTime <= now) {
                 cout << "Exporting flow!!! " << endl; 
                if(difftime(now, (*it)->lastUpdate) >= m_cxt->getNFExpireInterval()) {
                    if((*it)->isNetworkFlow) {
                         m_netflowPrcr->removeNetworkFlow((*it));
                    }
                    it = m_dfSet.erase(it);
                } else {
                    if((*it)->isNetworkFlow) {
                         m_netflowPrcr->exportNetworkFlow((*it), now);
                    }
                    DataFlowObj* dfo = (*it);
                    it = m_dfSet.erase(it);
                    dfo->exportTime = utils::getExportTime(m_cxt);
                    m_dfSet.insert(dfo);
                }
                i++;
            }else {
               break;
            }
     }
     return i;
}

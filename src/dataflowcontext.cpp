#include "dataflowcontext.h"

using namespace dataflow;

DataFlowContext::DataFlowContext(SysFlowContext* cxt, SysFlowWriter* writer, process::ProcessContext* processCxt) {
     m_cxt = cxt;
     m_netflowCxt = new networkflow::NetworkFlowContext(cxt, writer, processCxt);
}

DataFlowContext::~DataFlowContext() {
   //clearTables();
   if(m_netflowCxt != NULL) {
      delete m_netflowCxt;
   }
}


int DataFlowContext::handleDataEvent(sinsp_evt* ev, NFOpFlags flag) {
    sinsp_fdinfo_t * fdinfo = ev->get_fd_info();
    if(fdinfo == NULL && ev->get_type() == PPME_SYSCALL_SELECT_X) {
       return 0;
    }

    if(fdinfo == NULL) {
       cout << "Uh oh!!! Event: " << ev->get_name() << " doesn't have an fdinfo associated with it! ErrorCode: " << utils::getSyscallResult(ev) << endl;
       return 1;
    }
    if(fdinfo->is_ipv4_socket() || fdinfo->is_ipv6_socket()) {
      return m_netflowCxt->handleNetFlowEvent(ev, flag);
    }
    return 2;
}

void DataFlowContext::clearTables() {
    m_netflowCxt->clearNetFlows();
}

int DataFlowContext::checkForExpiredRecords() {
    return m_netflowCxt->checkForExpiredFlows();
}

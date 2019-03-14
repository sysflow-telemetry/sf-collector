#include "dataflow.h"

int dataflow::handleDataEvent(Context* cxt, sinsp_evt* ev, NFOpFlags flag) {
    sinsp_fdinfo_t * fdinfo = ev->get_fd_info();
    if(fdinfo == NULL) {
       cout << "Uh oh!!! Event: " << ev->get_name() << " doesn't have an fdinfo associated with it! " << endl;
       return 1;
    }
    if(fdinfo->is_ipv4_socket() || fdinfo->is_ipv6_socket()) {
      return networkflow::handleNetFlowEvent(cxt, ev, flag);
    }
    return 2;
}

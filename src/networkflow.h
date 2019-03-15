#ifndef _SF_NET_FLOW_
#define _SF_NET_FLOW_
#include <ctime>

#include <sinsp.h>
#include "op_flags.h"
#include "sysflow/sysflow.hh"
using namespace sysflow;
class Context;
namespace networkflow {

    struct NFKey {
       uint32_t ip1;
       uint16_t port1;
       uint32_t ip2;
       uint16_t port2;
    };
   
    class NetFlowObj {
        public:
            time_t expireTime;
            time_t lastUpdate;
            NetworkFlow netflow;
    };
    int handleNetFlowEvent(Context* cxt, sinsp_evt* ev, NFOpFlags flag);
    void writeNetFlow(Context* cxt, NetFlowObj* nf);
}
#endif

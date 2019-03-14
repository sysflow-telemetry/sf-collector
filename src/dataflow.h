#ifndef _SF_DATA_FLOW_
#define _SF_DATA_FLOW_
#include <sinsp.h>
#include "context.h"
#include "op_flags.h"
#include "networkflow.h"

namespace dataflow {
    int handleDataEvent(Context* cxt, sinsp_evt* ev, NFOpFlags flag);
}

#endif

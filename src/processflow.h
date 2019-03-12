#ifndef _SF_PROC_FLOW_
#define _SF_PROC_FLOW_
#include <sinsp.h>
#include "sysflow/sysflow.hh"
#include "context.h"
#include "process.h"
#include "utils.h"

using namespace sysflow;

void writeCloneEvent(Context* cxt, sinsp_evt* ev);
void writeExitEvent(Context* cxt, sinsp_evt* ev);
void writeExecEvent(Context* cxt, sinsp_evt* ev);

#endif

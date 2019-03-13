#ifndef _SF_PROC_
#define _SF_PROC_
#include <sinsp.h>
#include "sysflow/sysflow.hh"
#include "context.h"
#include "utils.h"
#include "container.h"

using namespace sysflow;
namespace process {
    void updateProcess(Context* cxt, Process* proc, sinsp_evt* ev, ActionType actType);
    void writeProcess(Context* cxt, Process* proc);
    Process* createProcess(Context* cxt, sinsp_threadinfo* mainthread, sinsp_evt* ev, ActionType actType);
    Process* getProcess(Context* cxt, sinsp_evt* ev, ActionType actType, bool& created);
}
#endif

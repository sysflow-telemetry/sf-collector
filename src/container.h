#ifndef _SF_CONT_
#define _SF_CONT_
#include <sinsp.h>
#include "sysflow/sysflow.hh"
#include "context.h"
#include "hashtables.h"

using namespace sysflow;
namespace container {
    Container*  createContainer(Context* cxt, sinsp_evt* ev); 
    void writeContainer(Context* cxt, Container* container);
    Container* getContainer(Context* cxt, sinsp_evt* ev);
}
#endif

#ifndef _SF_HEADER_
#define _SF_HEADER_
#include <string>

#include "sysflow/sysflow.hh"
#include "context.h"

using namespace std;
using namespace sysflow;

void writeHeader(Context* cxt, string exporterID);

#endif

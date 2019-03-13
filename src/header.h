#ifndef _SF_HEADER_
#define _SF_HEADER_
#include <string>

#include "sysflow/sysflow.hh"

class Context;

using namespace std;
using namespace sysflow;
namespace header {
    void writeHeader(Context* cxt);
}
#endif

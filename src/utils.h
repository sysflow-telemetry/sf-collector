#ifndef _SF_UTILS_
#define _SF_UTILS_

#include <fstream>
#include <sinsp.h>
#include <string>
#include "avro/ValidSchema.hh"
#include "avro/Compiler.hh"
#include "avro/DataFile.hh"
#include "avro/Encoder.hh"
#include "avro/Decoder.hh"

class Context;
using namespace std;
namespace utils {
    string getUserName(Context* cxt, uint32_t uid);
    string getGroupName(Context* cxt, uint32_t gid);
    bool isInContainer(sinsp_evt* ev);
    int64_t getSyscallResult(sinsp_evt* ev);
    avro::ValidSchema loadSchema(const char* filename);
}
#endif

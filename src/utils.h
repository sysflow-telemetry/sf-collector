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
#include  <time.h>
#include "sysflowcontext.h"
#include <openssl/sha.h>
#include "boost/any.hpp"
#include "sysflow/sysflow.hh"

typedef boost::array<uint8_t, 20> FOID;
using namespace std;
using namespace sysflow;
struct NFKey;
namespace utils {
    string getUserName(SysFlowContext* cxt, uint32_t uid);
    string getGroupName(SysFlowContext* cxt, uint32_t gid);
    bool isInContainer(sinsp_evt* ev);
    int64_t getSyscallResult(sinsp_evt* ev);
    avro::ValidSchema loadSchema(const char* filename);
    time_t getExportTime(SysFlowContext* cxt);
    NFKey* getNFDelKey();
    NFKey* getNFEmptyKey();
    OID* getOIDDelKey();
    OID* getOIDEmptyKey();
    void generateFOID(string key, FOID* foid);

    inline time_t getCurrentTime(SysFlowContext* cxt) {
        if(cxt->isOffline()) {
            return (cxt->timeStamp)/1000000000;   
        } 
        return time(NULL);
     }
    inline uint64_t getSysdigTime(SysFlowContext* cxt) {
        if(cxt->isOffline()) {
            return cxt->timeStamp;   
        } 
        return sinsp_utils::get_current_time_ns();
     }
}
#endif

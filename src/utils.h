/** Copyright (C) 2019 IBM Corporation.
*
* Authors:
* Frederico Araujo <frederico.araujo@ibm.com>
* Teryl Taylor <terylt@ibm.com>
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
**/

#ifndef _SF_UTILS_
#define _SF_UTILS_
#include <fstream>
#include <sinsp.h>
#include <string>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include "avro/ValidSchema.hh"
#include "avro/Compiler.hh"
#pragma GCC diagnostic pop
//#include "avro/DataFile.hh"
//#include "avro/Encoder.hh"
//#include "avro/Decoder.hh"
#include  <time.h>
#include "sysflowcontext.h"
#include <openssl/sha.h>
#include "boost/any.hpp"
#include "sysflow.h"
#ifdef SYSPORTER_BOOST
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#else 
#include <filesystem>
namespace fs = std::filesystem;
#endif

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
    string getPath(sinsp_evt* ev, string paraName);
    string getAbsolutePath(sinsp_threadinfo* ti, int64_t dirfd, string fileName);
    string getAbsolutePath(sinsp_threadinfo* ti, string fileName);
    int64_t getFD(sinsp_evt* ev, string paraName);
    inline string getCanonicalPath(string fileName) {
        fs::path p(fileName);
        p = weakly_canonical(p);
        return p.string();
    }

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

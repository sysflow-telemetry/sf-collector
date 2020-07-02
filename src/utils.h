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
#include "avro/Compiler.hh"
#include "avro/ValidSchema.hh"
#include "ghc/fs_std.hpp"
#include "sysflow.h"
#include "sysflowcontext.h"
#include <ctime>
#include <fstream>
#include <openssl/sha.h>
#include <string>

using sysflow::OID;

typedef std::array<uint8_t, 20> FOID;
struct NFKey;

namespace utils {
avro::ValidSchema loadSchema(const char *filename);
time_t getExportTime(context::SysFlowContext *cxt);
NFKey *getNFDelKey();
NFKey *getNFEmptyKey();
OID *getOIDDelKey();
OID *getOIDEmptyKey();
void generateFOID(const string &key, FOID *foid);
fs::path getCanonicalPath(const string &fileName);

inline time_t getCurrentTime(context::SysFlowContext *cxt) {
  if (cxt->isOffline()) {
    return (cxt->timeStamp) / 1000000000;
  }
  return time(nullptr);
}

inline uint64_t getCurrentTimeNS()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

    return tv.tv_sec * (uint64_t) 1000000000 + tv.tv_usec * 1000;
}



inline uint64_t getSystemTime(context::SysFlowContext *cxt) {
  if (cxt->isOffline()) {
    return cxt->timeStamp;
  }
  return getCurrentTimeNS();
}
} // namespace utils
#endif

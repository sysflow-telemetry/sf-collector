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
#include <json/json.h>
#include <openssl/sha.h>
#include <sinsp.h>
#include <sstream>
#include <string>

#define SCH_FIELDS_STR "fields"
#define SCH_TYPE_STR "type"
#define SCH_NAME_STR "name"
#define SCH_SFHEADER_STR "SFHeader"
#define SCH_VERSION_STR "version"
#define SCH_DEFAULT_STR "default"

using sysflow::OID;

typedef std::array<uint8_t, 20> FOID;
struct NFKey;

namespace utils {
static const std::string EMPTY_STR = "";
int64_t getFlags(sinsp_evt *ev);
bool isCloneThreadSet(sinsp_evt *ev);
int64_t getFD(sinsp_evt *ev);
bool isMapAnonymous(sinsp_evt *ev);
int64_t getIntParam(sinsp_evt *ev, std::string pname);
std::string getUserName(context::SysFlowContext *cxt, std::string& containerId, uint32_t uid);
std::string getGroupName(context::SysFlowContext *cxt, std::string& containerId, uint32_t gid);
bool isInContainer(sinsp_evt *ev);
int64_t getSyscallResult(sinsp_evt *ev);
avro::ValidSchema loadSchema();
time_t getExportTime(context::SysFlowContext *cxt);
NFKey *getNFDelKey();
NFKey *getNFEmptyKey();
OID *getOIDDelKey();
OID *getOIDEmptyKey();
void generateFOID(const string &key, FOID *foid);
std::string getPath(sinsp_evt *ev, const std::string &paraName);
fs::path getCanonicalPath(const std::string &fileName);
std::string getAbsolutePath(sinsp_threadinfo *ti, int64_t dirfd,
                       const std::string &fileName);
std::string getAbsolutePath(sinsp_threadinfo *ti, const std::string &fileName);
int64_t getFD(sinsp_evt *ev, const std::string &paraName);
int64_t getSchemaVersion();

inline time_t getCurrentTime(context::SysFlowContext *cxt) {
  if (cxt->isOffline()) {
    return (cxt->timeStamp) / 1000000000;
  }
  return time(nullptr);
}
inline uint64_t getSysdigTime(context::SysFlowContext *cxt) {
  if (cxt->isOffline()) {
    return cxt->timeStamp;
  }
  return sinsp_utils::get_current_time_ns();
}

#define CHAR_MAP_STR "0123456789abcdef"

inline char *itoa(int val, int base) {

  static char buf[32] = {0};

  int i = 30;
  bool neg = (val < 0);
  if (neg) {
    val = -val;
  }
  for (; val && i; --i, val /= base) {
    buf[i] = CHAR_MAP_STR[val % base];
  }
  if (neg) {
    buf[i] = '-';
    return &buf[i];
  }
  return &buf[i + 1];
}
} // namespace utils
#endif

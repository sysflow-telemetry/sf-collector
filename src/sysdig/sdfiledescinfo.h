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
#ifndef SF_SD_FILEINFO
#define SF_SD_FILEINFO
#include "api/sfinspector.h"
#include "logger.h"
#include <sinsp.h>
#include <string>
namespace sysdig {

class SDFileDescInfo : public api::SysFlowFileDescInfo {
private:
  sinsp_fdinfo_t *fdinfo;
  sinsp_evt *ev;
  DEFINE_LOGGER();

public:
  SDFileDescInfo();
  virtual ~SDFileDescInfo();
  inline void init(sinsp_evt *e, sinsp_fdinfo_t *f) {
    fdinfo = f;
    ev = e;
  }
  int32_t getOpenFlag();
  bool isIPSocket();
  bool isIPv6Socket(); 
  char getFileType(); 
  std::string getName();
  uint32_t getSIPv4();
  uint32_t getDIPv4();
  uint32_t getSPort();
  uint32_t getDPort();
  int32_t getProtocol();
  void printIPTupleDebug();
};

} // namespace sysdig

#endif

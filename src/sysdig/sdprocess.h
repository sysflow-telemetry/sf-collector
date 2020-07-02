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
#ifndef SF_SD_PROCESS
#define SF_SD_PROCESS
#include "api/sfinspector.h"
#include "sysdig/sdcont.h"
#include "sysdig/sdutils.h"

#include <sinsp.h>

namespace sysdig {
class SDProcess : public api::SysFlowProcess {
private:
  sinsp_threadinfo *ti;
  sinsp_threadinfo *mainthread;
  sinsp_container_info::ptr_t container;
  sinsp *inspector;
  SDContainer cont;

public:
  SDProcess();
  SDProcess(sinsp_threadinfo *t, sinsp *inspect);
  ~SDProcess();
  inline void init(sinsp_threadinfo *t, sinsp *inspect) {
    ti = t;
    mainthread = ti->get_main_thread();
    inspector = inspect;
  }
  api::SysFlowContainer *getContainer();
  std::string getContainerID();
  uint64_t getCreateTS();
  uint64_t getHostPID();
  std::string getExe();
  std::string getArgs();
  bool hasParent();
  int64_t getUID();
  int64_t getGID();
  int64_t getParentPID();
  int64_t getParentCreateTS();
  std::string getUserName();
  std::string getGroupName();
  bool isInContainer();
  bool getTTY();
};
} // namespace sysdig
#endif

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
#ifndef SF_SD_EVENT
#define SF_SD_EVENT

#include "api/sfinspector.h"
#include "op_flags.h"
#include "sysdig/sdfiledescinfo.h"
#include "sysdig/sdprocess.h"
#include "sysdig/sdutils.h"
#include "sysdig/sysdig_calls.h"
#include <sinsp.h>
#include <string>
#include <utility>
#define SF_UID "uid"

using namespace std;

namespace sysdig {

class SDEvent : public api::SysFlowEvent {
private:
  sinsp_threadinfo *ti;
  sinsp_fdinfo_t *fdinfo;
  sinsp *inspector;
  SDProcess process;
  SDFileDescInfo finfo;

public:
  sinsp_evt *ev;
  SDEvent();
  virtual ~SDEvent();
  inline void init(sinsp *inspect) {
    opFlag = OP_NONE;
    sysCallEnter = false;
    switch (ev->get_type()) {
      SD_EXECVE_EXIT
      SD_CLONE_EXIT
      SD_PROCEXIT_E_X
      SD_OPEN_EXIT
      SD_ACCEPT_EXIT
      SD_CONNECT_EXIT
      SD_SEND_EXIT
      SD_RECV_EXIT
      SD_CLOSE_EXIT
      SD_SETNS_EXIT
      SD_MKDIR_EXIT
      SD_RMDIR_EXIT
      SD_LINK_EXIT
      SD_UNLINK_EXIT
      SD_SYMLINK_EXIT
      SD_RENAME_EXIT
      SD_SETUID_ENTER
      SD_SETUID_EXIT
      SD_SHUTDOWN_EXIT
      SD_MMAP_EXIT
    default: {
      return;
    }
    }
    // ev = e;
    inspector = inspect;
    ti = ev->get_thread_info();
    // fdinfo = ev->get_fd_info();
    process.init(ti, inspect);
    // finfo.init(ev, fdinfo);
  }
  bool isSysCallEnter();
  int64_t getTS();
  int64_t getTID();
  int64_t getSysCallResult();
  int64_t getFlags();

  bool isMapAnonymous();
  std::string getCWD();
  std::string getName();
  bool isMainThread();
  bool isCloneThreadSet();
  std::string getContainerID();

  api::SysFlowProcess *getProcess();
  bool isInContainer();
  int64_t getFD();
  std::string getUIDFromParameter();
  api::SysFlowFileDescInfo *getFileDescInfo();
  std::pair<std::string, std::string> getLinkPaths();
  std::string getPathForFileEvent();
};

} // namespace sysdig

#endif

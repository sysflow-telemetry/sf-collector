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
#ifndef SD_INSPECTOR
#define SD_INSPECTOR
#include "api/sfinspector.h"
#include "sysdig/sdevent.h"
#include <memory>
#include <sinsp.h>
#include <system_error>
#define SYSDIG_LOG "SYSDIG_LOG"

namespace sysdig {

class SysdigInspector : public api::SysFlowInspector {
private:
  SDEvent evt;
  sinsp *m_inspector;
  void initExporterID();

public:
  SysdigInspector(context::SysFlowContext *cxt);
  virtual ~SysdigInspector();
  int init();
  int next(api::SysFlowEvent **evt);
  int cleanup();
  std::string getLastError();
  std::unique_ptr<api::SysFlowProcess> getProcess(int64_t pid) {
    sinsp_threadinfo *ti = m_inspector->get_thread(pid, false, true);
    if (ti == nullptr) {
      return std::unique_ptr<api::SysFlowProcess>(nullptr);
    }
    std::unique_ptr<api::SysFlowProcess> p(new SDProcess(ti, m_inspector));
    return p;
  }
};

} // namespace sysdig

#endif

/** Copyright (C) 2022 IBM Corporation.
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

#ifndef _SF_K8S_EVT
#define _SF_K8S_EVT
#include "k8scontext.h"
#include "logger.h"
#include "syscall_defs.h"
#include "sysflow.h"
#include "sysflowwriter.h"
#include "utils.h"
#include "json/json.h"
#include <sinsp.h>

namespace k8sevent {
class K8sEventProcessor {
private:
  sfk8s::K8sContext *m_k8sCxt;
  writer::SysFlowWriter *m_writer;
  sysflow::K8sEvent m_k8sEvt;  
  DEFINE_LOGGER();

public:
  K8sEventProcessor(writer::SysFlowWriter *writer, sfk8s::K8sContext *k8sCxt);
  virtual ~K8sEventProcessor();
  int handleK8sEvent(sinsp_evt *ev);

private:
  sysflow::K8sComponent getK8sComponent(Json::Value &root);
  sysflow::K8sAction getAction(Json::Value &root);
};
} // namespace k8sevent
#endif

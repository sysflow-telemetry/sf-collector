/** Copyright (C) 2021 IBM Corporation.
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

#ifndef _SF_K8S_
#define _SF_K8S_
#include <string>

#include "datatypes.h"
#include "sysflow.h"
#include "sysflowcontext.h"
#include "sysflowwriter.h"
#include <sinsp.h>
#include <k8s.h>
#define K8S_TABLE_SIZE 100
//#define INCOMPLETE "incomplete"
//#define INCOMPLETE_IMAGE "incomplete:incomplete"

namespace sfk8s {
class K8sContext {
private:
  PodTable m_pods;
  context::SysFlowContext *m_cxt;
  writer::SysFlowWriter *m_writer;
  std::shared_ptr<PodObj> createPod(const k8s_pod_t* p, const k8s_state_t& k8sState);

public:
  K8sContext(context::SysFlowContext *cxt, writer::SysFlowWriter *writer);
  virtual ~K8sContext();
  std::shared_ptr<PodObj> getPod(sinsp_threadinfo *ti);
  std::shared_ptr<PodObj> getPod(const string &id);
  bool exportPod(const string &id);
  int derefPod(const string &id);
  void clearAllPods();
  void clearPods();
  inline int getSize() { return m_pods.size(); }
};
} // namespace k8s
#endif
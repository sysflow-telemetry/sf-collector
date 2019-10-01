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

#ifndef _SF_NET_FLOW_
#define _SF_NET_FLOW_
#include "datatypes.h"
#include "logger.h"
#include "op_flags.h"
#include "processcontext.h"
#include "sysflow.h"
#include "sysflowcontext.h"
#include "sysflowwriter.h"
#include <ctime>
#include <sinsp.h>
using namespace sysflow;
namespace networkflow {

class NetworkFlowProcessor {
private:
  SysFlowContext *m_cxt;
  process::ProcessContext *m_processCxt;
  SysFlowWriter *m_writer;
  DataFlowSet *m_dfSet;
  DEFINE_LOGGER();
  void canonicalizeKey(sinsp_fdinfo_t *fdinfo, NFKey *key, uint64_t tid,
                       uint64_t fd);
  void canonicalizeKey(NetFlowObj *nf, NFKey *key);
  void populateNetFlow(NetFlowObj *nf, OpFlags flag, sinsp_evt *ev,
                       ProcessObj *proc);
  void updateNetFlow(NetFlowObj *nf, OpFlags flag, sinsp_evt *ev);
  void processExistingFlow(sinsp_evt *ev, ProcessObj *proc, OpFlags flag,
                           NFKey key, NetFlowObj *nf);
  void processNewFlow(sinsp_evt *ev, ProcessObj *proc, OpFlags flag, NFKey key);
  void removeAndWriteNetworkFlow(ProcessObj *proc, NetFlowObj **nf, NFKey *key);
  void removeNetworkFlow(ProcessObj *proc, NetFlowObj **nf, NFKey *key);
  int32_t getProtocol(scap_l4_proto proto);
  int removeNetworkFlowFromSet(NetFlowObj **nfo, bool deleteNetFlow);
  void removeAndWriteRelatedFlows(ProcessObj *proc, NFKey *key, uint64_t endTs);

public:
  NetworkFlowProcessor(SysFlowContext *cxt, SysFlowWriter *writer,
                       process::ProcessContext *procCxt, DataFlowSet *dfSet);
  virtual ~NetworkFlowProcessor();
  int handleNetFlowEvent(sinsp_evt *ev, OpFlags flag);
  inline int getSize() { return m_processCxt->getNumNetworkFlows(); }
  int removeAndWriteNFFromProc(ProcessObj *proc, int64_t tid);
  void removeNetworkFlow(DataFlowObj *dfo);
  void exportNetworkFlow(DataFlowObj *dfo, time_t now);
};
} // namespace networkflow
#endif

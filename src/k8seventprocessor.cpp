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

#include "k8seventprocessor.h"
#include <cassert>

using k8sevent::K8sEventProcessor;

CREATE_LOGGER(K8sEventProcessor, "sysflow.k8sevent");

K8sEventProcessor::K8sEventProcessor(writer::SysFlowWriter *writer,
                                     sfk8s::K8sContext *k8sCxt) {
  m_writer = writer;
  m_k8sCxt = k8sCxt;
}

K8sEventProcessor::~K8sEventProcessor() = default;

int K8sEventProcessor::handleK8sEvent(sinsp_evt *ev) {
  int res = 1;
  /*int len = ev->len;
  int headerLen = sizeof(struct ppm_evt_hdr);
  int hdrPayloadLen = headerLen + sizeof(uint16_t);
  if(len > hdrPayloadLen) {
    uint16_t* payloadLen = (uint16_t*)((char*)ev + headerLen);
    if(len >= hdrPayloadLen + *payloadLen) {
       char* p = (char*)((char*) ev + hdrPayloadLen);
       std::string payload(&p[0], &p[0] + *payloadLen);
       std::cout << "K8s payload: " << payload << std::endl;
    }

  }*/
  sinsp_evt_param *parinfo = ev->get_param(0);
  std::cout << "K8s Param length is: " << parinfo->m_len << std::endl;
  std::string payload(parinfo->m_val, parinfo->m_len);
  std::cout << "K8s payload: " << payload << std::endl;
  m_k8sEvt.message = payload;
  m_k8sEvt.ts = ev->get_ts();
  m_writer->writeK8sEvent(&m_k8sEvt);
  return res;
}

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

sysflow::K8sComponent K8sEventProcessor::getK8sComponent(Json::Value &root) {
  const Json::Value &kind = root["kind"];
  sysflow::K8sComponent comp = sysflow::K8sComponent::K8S_UNKNOWN;
  if (!kind.isNull() && kind.isString()) {
    std::string type = kind.asString();
    if (type == "Namespace") {
      comp = sysflow::K8sComponent::K8S_NAMESPACES;
    } else if (type == "Node") {
      comp = sysflow::K8sComponent::K8S_NODES;
    } else if (type == "Pod") {
      comp = sysflow::K8sComponent::K8S_PODS;
    } else if (type == "ReplicationController") {
      comp = sysflow::K8sComponent::K8S_REPLICATIONCONTROLLERS;
    } else if (type == "ReplicaSet") {
      comp = sysflow::K8sComponent::K8S_REPLICASETS;
    } else if (type == "Service") {
      comp = sysflow::K8sComponent::K8S_SERVICES;
    } else if (type == "DaemonSet") {
      comp = sysflow::K8sComponent::K8S_DAEMONSETS;
    } else if (type == "Deployment") {
      comp = sysflow::K8sComponent::K8S_DEPLOYMENTS;
    } else if (type == "EventList") {
      comp = sysflow::K8sComponent::K8S_EVENTS;
    } else {
      SF_WARN(m_logger, "Unknown k8s component type " << type << ". Ignoring")
    }
  } else {
    SF_WARN(m_logger, "Unknown k8s component type not found in JSON")
  }
  return comp;
}

sysflow::K8sAction K8sEventProcessor::getAction(Json::Value &root) {
  Json::Value evtype = root["type"];
  sysflow::K8sAction action = sysflow::K8sAction::K8S_COMPONENT_UNKNOWN;
  if (!evtype.isNull() && evtype.isString()) {
    const std::string &et = evtype.asString();
    if (!et.empty()) {
      switch (et[0]) {
      case 'A': {
        action = sysflow::K8sAction::K8S_COMPONENT_ADDED;
        break;
      }
      case 'M': {
        action = sysflow::K8sAction::K8S_COMPONENT_MODIFIED;
        break;
      }
      case 'D': {
        action = sysflow::K8sAction::K8S_COMPONENT_DELETED;
        break;
      }
      case 'N': {
        action = sysflow::K8sAction::K8S_COMPONENT_NONEXISTENT;
        break;
      }
      case 'E': {
        action = sysflow::K8sAction::K8S_COMPONENT_ERROR;
        break;
      }
      default: {
        break;
      }
      }
    }
  }
  return action;
}

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
  Json::Value root;
  Json::Reader reader;
  if (reader.parse(payload, root, false)) {
    m_k8sEvt.action = this->getAction(root);
    m_k8sEvt.kind = this->getK8sComponent(root);
    if (m_k8sEvt.action == sysflow::K8sAction::K8S_COMPONENT_MODIFIED &&
        (m_k8sEvt.kind == sysflow::K8sComponent::K8S_PODS ||
         m_k8sEvt.kind == sysflow::K8sComponent::K8S_SERVICES ||
         m_k8sEvt.kind == sysflow::K8sComponent::K8S_NODES)) {
      m_k8sCxt->updateCompState(m_k8sEvt.action, m_k8sEvt.kind, root);
    }
  }

  m_writer->writeK8sEvent(&m_k8sEvt);
  return res;
}

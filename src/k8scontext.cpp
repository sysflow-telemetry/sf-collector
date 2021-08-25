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

#include "k8scontext.h"

using sfk8s::K8sContext;

K8sContext::K8sContext(context::SysFlowContext *cxt,
                       writer::SysFlowWriter *writer)
    : m_pods(K8S_TABLE_SIZE) {
  m_cxt = cxt;
  m_writer = writer;
  m_pods.set_empty_key("0");
  m_pods.set_deleted_key("");
}

K8sContext::~K8sContext() { clearAllPods(); }

std::shared_ptr<PodObj> K8sContext::getPod(const string &id) {
  auto pod = m_pods.find(id);
  if (pod != m_pods.end()) {
    return pod->second;
  }
  return nullptr;
}

bool K8sContext::exportPod(const string &id) {
  bool exprt = false;
  auto pod = m_pods.find(id);
  if (pod != m_pods.end()) {
    if (!pod->second->written) {
      std::cout << "Writing pod marked not written " << pod->second->pod.name
                << std::endl;
      m_writer->writePod(&(pod->second->pod));
      pod->second->written = true;
      exprt = true;
    }
  }
  return exprt;
}

int K8sContext::derefPod(const string &id) {
  int result = 0;
  auto pod = m_pods.find(id);
  if (pod != m_pods.end()) {
    pod->second->refs--;
    result = pod->second->refs;
  }
  return result;
}

std::shared_ptr<PodObj> K8sContext::createPod(const k8s_pod_t *p,
                                              const k8s_state_t &k8sState) {
  std::cout << "Creating Pod object: " << p->get_name() << std::endl;
  std::shared_ptr<PodObj> pod = std::make_shared<PodObj>(
      p->get_uid(), p->get_name(), p->get_node_name(), p->get_host_ip(),
      p->get_internal_ip(), p->get_namespace(), p->get_restart_count());
  pod->pod.ts = utils::getSysdigTime(m_cxt);

  std::cout << "Getting pod labels!!" << std::endl;
  auto labels = p->get_labels();
  for (auto it = labels.begin(); it != labels.end(); it++) {
    pod->pod.labels.insert((*it));
  }
  std::cout << "Getting pod selectors!!" << std::endl;
  auto selectors = p->get_selectors();
  for (auto it = selectors.begin(); it != selectors.end(); it++) {
    pod->pod.selectors.insert((*it));
  }
  std::cout << "Getting pod service map!!" << std::endl;
  const k8s_state_t::pod_service_map &podServices =
      k8sState.get_pod_service_map();
  auto range = podServices.equal_range(p->get_uid());
  std::cout << "Retrieving Range!!" << std::endl;
  for (auto it = range.first; it != range.second; it++) {
    sysflow::Service srv;
    const k8s_service_t *service = it->second;
    srv.name = service->get_name();
    srv.id = service->get_uid();
    srv.namespace_ = service->get_namespace();
    utils::strToIP(service->get_cluster_ip().c_str(), srv.clusterIP);
    auto ports = service->get_port_list();
    std::cout << "Retrieving ports!!" << std::endl;
    for (auto i = ports.begin(); i != ports.end(); i++) {
      sysflow::Port port;
      port.port = (*i).m_port;
      port.proto = (*i).m_protocol;
      port.targetPort = (*i).m_target_port;
      port.nodePort = (*i).m_node_port;
      srv.portList.push_back(port);
    }
    std::cout << "Push service to pod!!!" << std::endl;
    pod->pod.services.push_back(srv);
  }
  std::cout << "Push service to pod!!!" << std::endl;
  return pod;
}

std::shared_ptr<PodObj> K8sContext::getPod(sinsp_threadinfo *ti) {

  std::shared_ptr<PodObj> pod(nullptr);
  std::cout << "Called Get Pod!!" << std::endl;
  if (ti->m_container_id.empty()) {
    std::cout << "Container Id empty.. return" << std::endl;
    return pod;
  }
  std::cout << "Call get_state()" << std::endl;
  const k8s_state_t &k8sState =
      m_cxt->getInspector()->m_k8s_client->get_state();
  std::cout << "Call get_pod()" << std::endl;
  const k8s_pod_t *p = k8sState.get_pod(ti->m_container_id);
  if (p == nullptr) {
    std::cout << "No pod in sysdig api for container.." << ti->m_container_id
              << std::endl;
    return pod;
  }
  std::cout << "Find pod with ID: " << p->get_uid() << std::endl;
  auto pitr = m_pods.find(p->get_uid());
  if (pitr != m_pods.end()) {
    if (pitr->second->written) {
      std::cout << "Returning pod with ID: " << p->get_uid() << std::endl;
      return pitr->second;
    }
    pod = pitr->second;
  }

  if (pod == nullptr) {
    std::cout << "Call create pod for: " << p->get_uid() << std::endl;
    pod = createPod(p, k8sState);
  }
  if (pod == nullptr) {
    return nullptr;
  }
  std::cout << "Add to hash map: " << p->get_uid() << std::endl;
  m_pods[p->get_uid()] = pod;
  std::cout << "Write!!: " << p->get_uid() << std::endl;
  m_writer->writePod(&(pod->pod));
  std::cout << "Return getPod: " << p->get_uid() << std::endl;
  pod->written = true;
  return pod;
}

void K8sContext::clearPods() {
  for (auto it = m_pods.begin(); it != m_pods.end(); ++it) {
    if (it->second->refs == 0) {
      std::cout << "Removing pod from pod map " << it->second->pod.name
                << std::endl;
      m_pods.erase(it);
    } else {
      std::cout << "Marking pod as written=false " << it->second->pod.name
                << std::endl;
      it->second->written = false;
    }
  }
}

void K8sContext::clearAllPods() { m_pods.clear(); }
void K8sContext::updateAndWritePodState(std::string &uid) {
  std::cout << "Update and write pod state for modified pod: " << uid
            << std::endl;
  const k8s_state_t &k8sState =
      m_cxt->getInspector()->m_k8s_client->get_state();
  const k8s_pod_t *pod =
      k8sState.get_component<k8s_pods, k8s_pod_t>(k8sState.get_pods(), uid);
  if (pod != nullptr) {
    std::shared_ptr<PodObj> podO = getPod(uid);
    if (podO == nullptr) {
      std::cout
          << "Unable to find pod object in cache... ignoring pod modification. "
          << uid << std::endl;
      return;
    }
    std::shared_ptr<PodObj> podObj = createPod(pod, k8sState);
    if (podObj != nullptr) {
      podObj->refs = podO->refs;
      m_pods[pod->get_uid()] = podObj;
      std::cout << "Write!!: " << pod->get_uid() << std::endl;
      m_writer->writePod(&(podObj->pod));
      podObj->written = true;
    } else {
      std::cout << "Unable to find pod with uid" << uid << std::endl;
    }
  } else {
    std::cout << "Unable to find pod with uid " << uid
              << " in global k8s state. " << std::endl;
  }
}

void K8sContext::updateCompState(sysflow::K8sAction action,
                                 sysflow::K8sComponent comp,
                                 const Json::Value &root) {
  if (action != sysflow::K8sAction::K8S_COMPONENT_MODIFIED) {
    return;
  }
  for (const Json::Value &item : root["items"]) {
    Json::Value uid = item["uid"];
    if (!uid.isNull()) {
      switch (comp) {
      case sysflow::K8sComponent::K8S_PODS: {
        std::string u = uid.asString();
        updateAndWritePodState(u);
        break;
      }
      default: {
        std::cout << "Don't support updates on k8s component type: "
                  << (int)comp << std::endl;
      }
      }
    }
  }
}

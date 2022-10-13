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

#include "containercontext.h"

using container::ContainerContext;
using sysflow::ContainerType;

void ContainerContext::setContainer(ContainerObj **cont,
                                    sinsp_container_info::ptr_t container) {
  SF_DEBUG(m_logger, "Setting container info. Name: " << container->m_name)
  (*cont)->cont.name = container->m_name;
  (*cont)->cont.image = container->m_image + ":" + container->m_imagetag;
  (*cont)->cont.id = container->m_id;
  (*cont)->cont.imageid = container->m_imageid;
  (*cont)->cont.type = static_cast<ContainerType>(container->m_type);
  (*cont)->cont.privileged = container->m_privileged;
}

ContainerContext::ContainerContext(context::SysFlowContext *cxt,
                                   writer::SysFlowWriter *writer,
                                   sfk8s::K8sContext *k8sCxt)
    : m_containers(CONT_TABLE_SIZE) {
  m_cxt = cxt;
  m_writer = writer;
  m_k8sCxt = k8sCxt;
  m_containers.set_empty_key("0");
  m_containers.set_deleted_key("");
}

ContainerContext::~ContainerContext() { clearAllContainers(); }

ContainerObj *ContainerContext::createContainer(sinsp_threadinfo *ti) {

  if (ti->m_container_id.empty()) {
    return nullptr;
  }

  const sinsp_container_info::ptr_t container =
      m_cxt->getInspector()->m_container_manager.get_container(
          ti->m_container_id);
  if (!container) {
    SF_DEBUG(m_logger, "Thread has container id, but no container object. ID: "
                           << ti->m_container_id)
    auto *cont = new ContainerObj();
    cont->cont.name = INCOMPLETE;
    cont->cont.image = INCOMPLETE_IMAGE;
    cont->cont.id = ti->m_container_id;
    cont->incomplete = true;
    return cont;
  }

  auto *cont = new ContainerObj();
  setContainer(&cont, container);
  if (cont->cont.name.compare(INCOMPLETE) == 0 ||
      cont->cont.image.compare(INCOMPLETE_IMAGE) == 0) {
    cont->incomplete = true;
  }

  if (m_cxt->isK8sEnabled()) {
    std::shared_ptr<PodObj> pod = m_k8sCxt->getPod(ti);
    if (pod != nullptr) {
      SF_DEBUG(m_logger, "Setting pod id to " << pod->pod.id
                                              << " for container "
                                              << ti->m_container_id)
      cont->cont.podId.set_string(pod->pod.id);
      pod->refs++;
    } else {
      cont->cont.podId.set_null();
    }
  }

  return cont;
}

ContainerObj *ContainerContext::getContainer(const string &id) {
  ContainerTable::iterator cont = m_containers.find(id);
  if (cont != m_containers.end()) {
    return cont->second;
  }
  return nullptr;
}

bool ContainerContext::exportContainer(const string &id) {
  bool exprt = false;
  ContainerTable::iterator cont = m_containers.find(id);
  if (cont != m_containers.end()) {
    if (m_cxt->isK8sEnabled() && !cont->second->cont.podId.is_null()) {
      m_k8sCxt->exportPod(cont->second->cont.podId.get_string());
    }
    if (!cont->second->written) {
      m_writer->writeContainer(&(cont->second->cont));
      cont->second->written = true;
      exprt = true;
    }
  }
  return exprt;
}

int ContainerContext::derefContainer(const string &id) {
  int result = 0;
  ContainerTable::iterator cont = m_containers.find(id);
  if (cont != m_containers.end()) {
    cont->second->refs--;
    result = cont->second->refs;
  }
  return result;
}

ContainerObj *ContainerContext::getContainer(sinsp_threadinfo *ti) {

  if (ti->m_container_id.empty()) {
    return nullptr;
  }

  ContainerObj *ct = nullptr;
  ContainerTable::iterator cont = m_containers.find(ti->m_container_id);
  if (cont != m_containers.end()) {
    if (cont->second->written && !cont->second->incomplete) {
      return cont->second;
    }

    const sinsp_container_info::ptr_t container =
        m_cxt->getInspector()->m_container_manager.get_container(
            ti->m_container_id);
    if (!container) {
      // m_containers.erase(cont);
      // delete cont->second;
      return cont->second;
    }

    if (cont->second->written && cont->second->incomplete) {
      SF_DEBUG(m_logger,
               "Container is written and includes name: " << container->m_name);
      if (container->m_name.compare(INCOMPLETE) == 0 ||
          container->m_image.compare(INCOMPLETE) == 0) {
        return cont->second;
      } else {
        cont->second->incomplete = false;
      }
    }

    ct = cont->second;
    setContainer(&ct, container);
  }

  if (ct == nullptr) {
    ct = createContainer(ti);
  } else {
    if (m_cxt->isK8sEnabled()) {
      reupPod(ti, ct);
    }
  }

  if (ct == nullptr) {
    return nullptr;
  }

  m_containers[ct->cont.id] = ct;
  m_writer->writeContainer(&(ct->cont));
  ct->written = true;

  return ct;
}

void ContainerContext::reupPod(sinsp_threadinfo *ti, ContainerObj *cont) {
  if (!m_cxt->isK8sEnabled()) {
    return;
  }

  string podId = "";
  if (!cont->cont.podId.is_null()) {
    podId = cont->cont.podId.get_string();
  }

  if (!podId.empty()) {
    auto pod1 = m_k8sCxt->getPod(podId);
    if (pod1 != nullptr) {
      pod1->refs--;
    }
  }

  auto pod = m_k8sCxt->getPod(ti);
  if (pod != nullptr) {
    cont->cont.podId.set_string(pod->pod.id);
    pod->refs++;
  } else {
    cont->cont.podId.set_null();
  }
}

void ContainerContext::clearContainers() {
  for (ContainerTable::iterator it = m_containers.begin();
       it != m_containers.end(); ++it) {
    if (it->second->refs == 0) {
      if (m_cxt->isK8sEnabled() && !it->second->cont.podId.is_null()) {
        m_k8sCxt->derefPod(it->second->cont.podId.get_string());
      }
      m_containers.erase(it);
      delete it->second;
    } else {
      it->second->written = false;
    }
  }
}

void ContainerContext::clearAllContainers() {
  for (ContainerTable::iterator it = m_containers.begin();
       it != m_containers.end(); ++it) {
    delete it->second;
  }
}

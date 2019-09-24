/** Copyright (C) 2019 IBM Corporation.
*
* Authors:
* Teryl Taylor <terylt@ibm.com>
* Frederico Araujo <frederico.araujo@ibm.com>
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
using namespace container;

ContainerContext::ContainerContext(SysFlowContext* cxt, SysFlowWriter* writer) : m_containers(CONT_TABLE_SIZE) {
    m_cxt = cxt;
    m_writer = writer;
    m_containers.set_empty_key("0");
    m_containers.set_deleted_key("");
}

ContainerContext::~ContainerContext() {
    clearAllContainers();
}

ContainerObj*  ContainerContext::createContainer(sinsp_evt* ev) {
    sinsp_threadinfo* ti = ev->get_thread_info();
    
    if(ti->m_container_id.empty()) {
        return NULL;
    }

    sinsp_container_info* container = m_cxt->getInspector()->m_container_manager.get_container(ti->m_container_id);
    if(container == NULL) {
       return NULL;
    }
    ContainerObj* cont = new ContainerObj();
    cont->cont.name = container->m_name;
    cont->cont.image = container->m_image + "/" + container->m_imagetag;
    cont->cont.id = container->m_id;
    cont->cont.imageid = container->m_imageid;
    cont->cont.type = (ContainerType)container->m_type;
    cont->cont.privileged = container->m_privileged;
    return cont;
}


ContainerObj* ContainerContext::getContainer(string id) {
    ContainerTable::iterator cont = m_containers.find(id);
    if(cont != m_containers.end()) {
          return cont->second;
    }
    return NULL;
}

bool ContainerContext::exportContainer(string id) {
    bool exprt = false;
    ContainerTable::iterator cont = m_containers.find(id);
    if(cont != m_containers.end()) {
        if(!cont->second->written) {
            m_writer->writeContainer(&(cont->second->cont));
            cont->second->written = true;
            exprt = true;
       }
    }
    return exprt;
}

int ContainerContext::derefContainer(string id) {
    int result = 0;
    ContainerTable::iterator cont = m_containers.find(id);
    if(cont != m_containers.end()) {
          cont->second->refs--;
          result = cont->second->refs;
    }
    return result;
}


ContainerObj* ContainerContext::getContainer(sinsp_evt* ev) {
    sinsp_threadinfo* ti = ev->get_thread_info();
    
    if(ti->m_container_id.empty()) {
        return NULL;
    }
    ContainerObj* ct = NULL;
    ContainerTable::iterator cont = m_containers.find(ti->m_container_id);
    if(cont != m_containers.end()) {
        if(cont->second->written) {
            return cont->second;
        }
        ct = cont->second;
    }
    if(ct == NULL) {
        ct = createContainer(ev);
    }
    m_containers[ct->cont.id] = ct;
    m_writer->writeContainer(&(ct->cont));
    ct->written = true;
    return ct;
}

void ContainerContext::clearContainers() {
   for(ContainerTable::iterator it = m_containers.begin(); it != m_containers.end(); ++it) {
       if(it->second->refs == 0) {
           m_containers.erase(it);
           delete it->second;
       }else {
         it->second->written = false;
      }
   }
}

void ContainerContext::clearAllContainers() {
   for(ContainerTable::iterator it = m_containers.begin(); it != m_containers.end(); ++it) {
        delete it->second;
   }
}



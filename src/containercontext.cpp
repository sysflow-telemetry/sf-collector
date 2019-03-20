#include "containercontext.h"
using namespace container;

ContainerContext::ContainerContext(SysFlowContext* cxt, SysFlowWriter* writer) : m_containers(CONT_TABLE_SIZE) {
    m_cxt = cxt;
    m_writer = writer;
    m_containers.set_empty_key("0");
    m_containers.set_deleted_key("");
}

ContainerContext::~ContainerContext() {
    clearContainers();
}

Container*  ContainerContext::createContainer(sinsp_evt* ev) {
    sinsp_threadinfo* ti = ev->get_thread_info();
    
    if(ti->m_container_id.empty()) {
        return NULL;
    }

    sinsp_container_info* container = m_cxt->getInspector()->m_container_manager.get_container(ti->m_container_id);
    if(container == NULL) {
       return NULL;
    }
    Container* cont = new Container();
    cont->name = container->m_name;
    cont->image = container->m_image + "/" + container->m_imagetag;
    cont->id = container->m_id;
    cont->imageid = container->m_imageid;
    cont->type = (ContainerType)container->m_type;
    return cont;
}


Container* ContainerContext::getContainer(sinsp_evt* ev) {
    sinsp_threadinfo* ti = ev->get_thread_info();
    
    if(ti->m_container_id.empty()) {
        return NULL;
    }
    ContainerTable::iterator cont = m_containers.find(ti->m_container_id);
    if(cont != m_containers.end()) {
        return cont->second;
    }
    Container* container = createContainer(ev);
    m_containers[container->id] = container;
    m_writer->writeContainer(container);
    return container;
}

void ContainerContext::clearContainers() {
   for(ContainerTable::iterator it = m_containers.begin(); it != m_containers.end(); ++it) {
       delete it->second;
   }
   m_containers.clear();
}

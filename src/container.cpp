#include "container.h"

Container*  container::createContainer(Context* cxt, sinsp_evt* ev) {
    sinsp_threadinfo* ti = ev->get_thread_info();
    
    if(ti->m_container_id.empty()) {
        return NULL;
    }

    sinsp_container_info* container = cxt->inspector->m_container_manager.get_container(ti->m_container_id);
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

void container::writeContainer(Context* cxt, Container* container) {
    cxt->flow.rec.set_Container(*container);
    cxt->numRecs++;
    cxt->dfw->write(cxt->flow);
}

Container* container::getContainer(Context* cxt, sinsp_evt* ev) {
    sinsp_threadinfo* ti = ev->get_thread_info();
    
    if(ti->m_container_id.empty()) {
        return NULL;
    }
    ContainerTable::iterator cont = cxt->conts.find(ti->m_container_id);
    if(cont != cxt->conts.end()) {
        return cont->second;
    }
    Container* container = createContainer(cxt, ev);
    cxt->conts[container->id] = container;
    writeContainer(cxt, container);
    return container;
}

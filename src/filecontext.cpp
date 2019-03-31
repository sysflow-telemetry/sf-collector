#include "filecontext.h"
using namespace file;

FileContext::FileContext(container::ContainerContext* containerCxt, SysFlowWriter* writer) {
    m_writer = writer;
    m_containerCxt = containerCxt;
}

FileContext::~FileContext() {
}


FileObj* FileContext::createFile(sinsp_evt* ev, SFObjectState state, string key) {
    sinsp_fdinfo_t * fdinfo = ev->get_fd_info();
    FileObj* f = new FileObj();
    f->key = key; 
    f->file.state = state;
    f->file.ts = ev->get_ts();
    utils::generateFOID(f->key, &(f->file.oid));
    f->file.path = fdinfo->m_name;
    f->file.restype = fdinfo->get_typechar();
    Container* cont = m_containerCxt->getContainer(ev);
    if(cont != NULL) {
       f->file.containerId.set_string(cont->id);
    }else {
       f->file.containerId.set_null();
    }
    return f;
}

FileObj* FileContext::getFile(sinsp_evt* ev, SFObjectState state, bool& created) {
    sinsp_threadinfo* ti = ev->get_thread_info();
    sinsp_fdinfo_t * fdinfo = ev->get_fd_info();
    created = true;
    string key = ti->m_container_id + fdinfo->m_name;
    FileTable::iterator f = m_files.find(key);
    FileObj* file = NULL;
    if(f != m_files.end()) {
        created = false;
        if(f->second->written) {
            return f->second;
        }
        file = f->second;
        file->file.state = SFObjectState::REUP;
    }
    if(file == NULL) {
        file = createFile(ev, state, key);
    }
    m_files[key] = file;
    m_writer->writeFile(&(file->file));
    file->written = true;
    return file;
}

void FileContext::clearFiles() {
   for(FileTable::iterator it = m_files.begin(); it != m_files.end(); ++it) {
          if(it->second->refs == 0) {
              FileObj* file = it->second;
              m_files.erase(it);
              delete file;
          } else {
              it->second->written = false;
          } 
   } 
}

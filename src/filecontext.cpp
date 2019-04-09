#include "filecontext.h"
using namespace file;

FileContext::FileContext(container::ContainerContext* containerCxt, SysFlowWriter* writer) {
    m_writer = writer;
    m_containerCxt = containerCxt;
    m_files.set_empty_key("-1");
    m_files.set_deleted_key("-2");
}

FileContext::~FileContext() {
    clearAllFiles();
}


FileObj* FileContext::createFile(sinsp_evt* ev, string path, char typechar, SFObjectState state, string key) {
    FileObj* f = new FileObj();
    f->key = key; 
    f->file.state = state;
    f->file.ts = ev->get_ts();
    utils::generateFOID(f->key, &(f->file.oid));
    f->file.path = path;
    f->file.restype = typechar;
    ContainerObj* cont = m_containerCxt->getContainer(ev);
    if(cont != NULL) {
       f->file.containerId.set_string(cont->cont.id);
    }else {
       f->file.containerId.set_null();
    }
    return f;
}
FileObj* FileContext::getFile(sinsp_evt* ev, SFObjectState state, bool& created) {
    sinsp_fdinfo_t * fdinfo = ev->get_fd_info();
    return getFile(ev, fdinfo->m_name, fdinfo->get_typechar(), state, created);
}
FileObj* FileContext::getFile(sinsp_evt* ev, string path, char typechar, SFObjectState state, bool& created) {
    sinsp_threadinfo* ti = ev->get_thread_info();
    //sinsp_fdinfo_t * fdinfo = ev->get_fd_info();
    created = true;
    string key = ti->m_container_id + path;
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
        file = createFile(ev, path, typechar, state, key);
    }
    m_files[key] = file;
    m_writer->writeFile(&(file->file));
    file->written = true;
    return file;
}

FileObj* FileContext::getFile(string key) {
    FileTable::iterator f = m_files.find(key);
    if(f != m_files.end()) {
        if(!f->second->written) {
            f->second->file.state = SFObjectState::REUP;
            m_writer->writeFile(&(f->second->file));
            f->second->written = true;
        }
        return f->second;
    }
    return NULL;
}

bool FileContext::exportFile(string key) {
    FileTable::iterator f = m_files.find(key);
    bool exprt = false;
    if(f != m_files.end()) {
        if(!f->second->written) {
            f->second->file.state = SFObjectState::REUP;
            m_writer->writeFile(&(f->second->file));
            f->second->written = true;
            exprt = true;
        }
    }
    return exprt;
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

void FileContext::clearAllFiles() {
   for(FileTable::iterator it = m_files.begin(); it != m_files.end(); ++it) {
       delete it->second;
   }
}


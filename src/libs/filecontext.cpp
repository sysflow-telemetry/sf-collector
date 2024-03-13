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

#include "filecontext.h"
#include <utility>

extern "C" {
#include "sidcache_api.h"
}

using file::FileContext;

FileContext::FileContext(container::ContainerContext *containerCxt,
                         writer::SysFlowWriter *writer) {
  m_writer = writer;
  m_containerCxt = containerCxt;
  m_files.set_empty_key("-1");
  m_files.set_deleted_key("-2");
}

FileContext::~FileContext() { clearAllFiles(); }

FileObj *FileContext::createFile(sinsp_evt *ev, std::string path, char typechar,
                                 SFObjectState state, std::string key) {
  auto *f = new FileObj();
  f->key = std::move(key);
  f->file.state = state;
  f->file.ts = ev->get_ts();
  utils::generateFOID(f->key, &(f->file.oid));
  f->file.path = std::move(path);
  f->file.restype = typechar;
  sinsp_threadinfo *ti = ev->get_thread_info();
  sinsp_threadinfo *mt = ti->get_main_thread();
  ContainerObj *cont = m_containerCxt->getContainer(mt);
  if (cont != nullptr) {
    f->file.containerId.set_string(cont->cont.id);
  } else {
    f->file.containerId.set_null();
  }
  if (ev->get_type() == PPME_SYSCALL_OPENAT_2_X ||
      ev->get_type() == PPME_SYSCALL_OPEN_X ||
      ev->get_type() == PPME_SYSCALL_OPENAT2_X) {
    uint32_t n = ev->get_num_params();
    printf("Openat called %s\n", f->file.path.c_str());
    int paramid = 0;
    if (ev->get_type() == PPME_SYSCALL_OPENAT_2_X && n >= 8) {
      paramid = 7;
    } else if (ev->get_type() == PPME_SYSCALL_OPEN_X ||
               ev->get_type() == PPME_SYSCALL_OPENAT2_X && n >= 7) {
      paramid = 6;
    }
    if (paramid != 0) {
      const sinsp_evt_param *p = ev->get_param(paramid);
      f->file.sid = *reinterpret_cast<int64_t *>(p->m_val);
      printf("Openat %s, SID: %d\n", f->file.path.c_str(), f->file.sid);
      char *label = sc_get_ctx(f->file.sid);
      if (label != nullptr) {
        f->file.selabel = std::string(label);
        printf("Openat %s, SID: %d, Label: %s\n", f->file.path.c_str(),
               f->file.sid, f->file.selabel.c_str());
      }
    }
  }
  return f;
}
FileObj *FileContext::getFile(sinsp_evt *ev, sinsp_fdinfo_t *fdinfo,
                              SFObjectState state, bool &created) {
  return getFile(ev, fdinfo->m_name, fdinfo->get_typechar(), state, created);
}
FileObj *FileContext::getFile(sinsp_evt *ev, const std::string &path,
                              char typechar, SFObjectState state,
                              bool &created) {
  sinsp_threadinfo *ti = ev->get_thread_info();
  created = true;
  std::string key;
  key.reserve(ti->m_container_id.length() + path.length());
  key += ti->m_container_id;
  key += path;
  FileTable::iterator f = m_files.find(key);
  FileObj *file = nullptr;
  if (f != m_files.end()) {
    created = false;
    if (f->second->file.selabel.empty()) {
      char *label = sc_get_ctx(f->second->file.sid);
      if (label != nullptr && strlen(label) > 0) {
        f->second->file.selabel = std::string(label);
        f->second->written = false;
      }
    }

    if (f->second->written) {
      return f->second;
    }
    file = f->second;
    file->file.state = SFObjectState::REUP;
  }
  if (file == nullptr) {
    file = createFile(ev, path, typechar, state, key);
  }
  m_files[key] = file;
  m_writer->writeFile(&(file->file));
  file->written = true;
  return file;
}

FileObj *FileContext::getFile(const std::string &key) {
  FileTable::iterator f = m_files.find(key);
  if (f != m_files.end()) {
    if (!f->second->written) {
      f->second->file.state = SFObjectState::REUP;
      m_writer->writeFile(&(f->second->file));
      f->second->written = true;
    }
    return f->second;
  }
  return nullptr;
}

FileObj *FileContext::exportFile(const std::string &key) {
  FileTable::iterator f = m_files.find(key);
  if (f != m_files.end()) {
    if (!f->second->written) {
      f->second->file.state = SFObjectState::REUP;
      m_writer->writeFile(&(f->second->file));
      f->second->written = true;
    }
    return f->second;
  }
  return nullptr;
}

void FileContext::clearFiles() {
  for (FileTable::iterator it = m_files.begin(); it != m_files.end(); ++it) {
    if (it->second->refs == 0) {
      FileObj *file = it->second;
      m_files.erase(it);
      delete file;
    } else {
      it->second->written = false;
    }
  }
}

void FileContext::clearAllFiles() {
  for (FileTable::iterator it = m_files.begin(); it != m_files.end(); ++it) {
    delete it->second;
  }
}

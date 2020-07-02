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
#include "sysdig/sdprocess.h"

using sysdig::SDProcess;

SDProcess::SDProcess() : ti(nullptr), mainthread(nullptr), inspector(nullptr) {}

SDProcess::SDProcess(sinsp_threadinfo *t, sinsp *inspect) { init(t, inspect); }

SDProcess::~SDProcess() {}

api::SysFlowContainer *SDProcess::getContainer() {
  if (ti->m_container_id.empty()) {
    return nullptr;
  }
  sinsp_container_info::ptr_t container =
      inspector->m_container_manager.get_container(ti->m_container_id);
  if (container == nullptr) {
    return nullptr;
  }
  cont.init(container);
  return &cont;
}

uint64_t SDProcess::getCreateTS() { return mainthread->m_clone_ts; }

uint64_t SDProcess::getHostPID() { return mainthread->m_pid; }

std::string SDProcess::getExe() {
  return (mainthread->m_exepath.empty())
             ? sysdig::getAbsolutePath(ti, mainthread->m_exe)
             : mainthread->m_exepath;
}

std::string SDProcess::getArgs() {
  int i = 0;
  std::string args;
  for (auto it = mainthread->m_args.begin(); it != mainthread->m_args.end();
       ++it) {
    if (i == 0) {
      args = *it;
    } else {
      args += " " + *it;
    }
    i++;
  }
  return args;
}

bool SDProcess::hasParent() {
  return mainthread->get_parent_thread() != nullptr;
}

int64_t SDProcess::getUID() { return mainthread->m_uid; }

int64_t SDProcess::getGID() { return mainthread->m_gid; }

int64_t SDProcess::getParentPID() { return mainthread->m_ptid; }

int64_t SDProcess::getParentCreateTS() {
  auto parent = mainthread->get_parent_thread();
  if (parent != nullptr) {
    return parent->m_clone_ts;
  }
  return -1;
}

std::string SDProcess::getUserName() {
  scap_userinfo *user = inspector->get_user(mainthread->m_uid);
  if (user != nullptr) {
    return user->name;
  }
  return std::string("");
}

std::string SDProcess::getGroupName() {
  if (mainthread->m_gid == 0xffffffff) {
    return std::string("");
  }
  unordered_map<uint32_t, scap_groupinfo *>::const_iterator it;

  it = inspector->m_grouplist.find(mainthread->m_gid);
  if (it == inspector->m_grouplist.end()) {
    return std::string("");
  }

  return it->second->name;
}

std::string SDProcess::getContainerID() { return ti->m_container_id; }

bool SDProcess::isInContainer() { return !ti->m_container_id.empty(); }

bool SDProcess::getTTY() { return mainthread->m_tty; }

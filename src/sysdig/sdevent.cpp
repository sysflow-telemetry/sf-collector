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

#include "sysdig/sdevent.h"

using sysdig::SDEvent;

SDEvent::SDEvent() : ti(nullptr), fdinfo(nullptr), inspector(nullptr) {}

SDEvent::~SDEvent() {}

bool SDEvent::isSysCallEnter() { return sysCallEnter; }

int64_t SDEvent::getTS() { return ev->get_ts(); }

int64_t SDEvent::getTID() { return ti->m_tid; }

int64_t SDEvent::getSysCallResult() { return sysdig::getSysCallResult(ev); }

int64_t SDEvent::getFlags() { return sysdig::getFlags(ev); }

bool SDEvent::isMapAnonymous() {
  int64_t flags = sysdig::getFlags(ev);
  return flags & PPM_MAP_ANONYMOUS;
}

std::string SDEvent::getCWD() { return ti->get_cwd(); }

std::string SDEvent::getName() { return ev->get_name(); }

bool SDEvent::isMainThread() { return ti->is_main_thread(); }

bool SDEvent::isCloneThreadSet() {
  int64_t cloneThread = sysdig::getFlags(ev);
  return cloneThread & PPM_CL_CLONE_THREAD;
}

std::string SDEvent::getContainerID() { return ti->m_container_id; }

api::SysFlowProcess *SDEvent::getProcess() { return &process; }

bool SDEvent::isInContainer() { return !ti->m_container_id.empty(); }

int64_t SDEvent::getFD() {
  sinsp_fdinfo_t *fi = ev->get_fd_info();
  int64_t fd = ev->get_fd_num();
  if (fi == nullptr && opFlag == OP_MMAP) {
    if (fd == sinsp_evt::INVALID_FD_NUM) {
      fd = sysdig::getFD(ev);
    }
  }
  return fd;
}

std::string SDEvent::getUIDFromParameter() {
  return ev->get_param_value_str(SF_UID);
}

api::SysFlowFileDescInfo *SDEvent::getFileDescInfo() {
  fdinfo = ev->get_fd_info();
  if (fdinfo != nullptr) {
    finfo.init(ev, fdinfo);
    return &finfo;
  } else if (opFlag == OP_MMAP) {
    int64_t fd = ev->get_fd_num();
    if (fd != sinsp_evt::INVALID_FD_NUM) {
      fdinfo = ti->get_fd(fd);
      if (fdinfo != nullptr) {
        finfo.init(ev, fdinfo);
        return &finfo;
      }
    } else {
      fd = sysdig::getFD(ev);
      if (!sysdig::isMapAnonymous(ev) && fd != -1) {
        // SF_INFO(m_logger, "FDs: " << fd );
        fdinfo = ti->get_fd(fd);
        if (fdinfo != nullptr) {
          finfo.init(ev, fdinfo);
          return &finfo;
        }
      }
    }
  }
  return nullptr;
}

std::pair<std::string, std::string> SDEvent::getLinkPaths() {
  std::string path1;
  std::string path2;
  if (opFlag == OP_LINK || opFlag == OP_RENAME) {
    path1 = sysdig::getPath(ev, "oldpath");
    path2 = sysdig::getPath(ev, "newpath");
    if (IS_AT_SC(ev->get_type())) {
      int64_t olddirfd;
      int64_t newdirfd;
      if (opFlag == OP_RENAME) {
        olddirfd = sysdig::getFD(ev, "olddirfd");
        newdirfd = sysdig::getFD(ev, "newdirfd");
      } else {
        olddirfd = sysdig::getFD(ev, "olddir");
        newdirfd = sysdig::getFD(ev, "newdir");
      }
      path1 = sysdig::getAbsolutePath(ti, olddirfd, path1);
      path2 = sysdig::getAbsolutePath(ti, newdirfd, path2);

    } else {
      path1 = sysdig::getAbsolutePath(ti, path1);
      path2 = sysdig::getAbsolutePath(ti, path2);
    }
  } else if (opFlag == OP_SYMLINK) {
    path1 = sysdig::getPath(ev, "target");
    path2 = sysdig::getPath(ev, "linkpath");
    if (IS_AT_SC(ev->get_type())) {
      uint64_t linkdirfd = sysdig::getFD(ev, "linkdirfd");
      path2 = sysdig::getAbsolutePath(ti, linkdirfd, path2);
    } else {
      path1 = sysdig::getAbsolutePath(ti, path1);
      path2 = sysdig::getAbsolutePath(ti, path2);
    }
  }
  return {path1, path2};
}

std::string SDEvent::getPathForFileEvent() {
  std::string fileName = (IS_UNLINKAT(ev->get_type()))
                             ? sysdig::getPath(ev, "name")
                             : sysdig::getPath(ev, "path");
  if (IS_AT_SC(ev->get_type())) {
    sinsp_evt_param *pinfo;
    pinfo = ev->get_param(1);
    assert(pinfo->m_len == sizeof(int64_t));
    int64_t dirfd = *reinterpret_cast<int64_t *>(pinfo->m_val);
    fileName = sysdig::getAbsolutePath(ti, dirfd, fileName);
  } else {
    fileName = sysdig::getAbsolutePath(ti, fileName);
  }
  return fileName;
}

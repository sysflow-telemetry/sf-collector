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

#include "utils.h"
#include "datatypes.h"
#include "logger.h"
#include "sysflowcontext.h"

static NFKey s_nfdelkey;
static NFKey s_nfemptykey;
static bool s_keysinit = false;
static OID s_oiddelkey;
static OID s_oidemptykey;

CREATE_LOGGER_2("sysflow.utils");

void initKeys() {
  s_nfdelkey.ip1 = 1;
  s_nfdelkey.ip2 = 1;
  s_nfdelkey.port1 = 1;
  s_nfdelkey.port2 = 1;
  s_nfemptykey.ip1 = 1;
  s_nfemptykey.ip2 = 0;
  s_nfemptykey.port1 = 1;
  s_nfemptykey.port2 = 1;
  s_oidemptykey.hpid = 2;
  s_oidemptykey.createTS = 2;
  s_oiddelkey.hpid = 1;
  s_oiddelkey.createTS = 1;
  s_keysinit = true;
}

void utils::generateFOID(const string &key, FOID *foid) {
  SHA1(reinterpret_cast<const unsigned char *>(key.c_str()), key.size(),
       foid->begin());
}

NFKey *utils::getNFEmptyKey() {
  if (!s_keysinit) {
    initKeys();
  }
  return &s_nfemptykey;
}

NFKey *utils::getNFDelKey() {
  if (!s_keysinit) {
    initKeys();
  }
  return &s_nfdelkey;
}

OID *utils::getOIDEmptyKey() {
  if (!s_keysinit) {
    initKeys();
  }
  return &s_oidemptykey;
}

OID *utils::getOIDDelKey() {
  if (!s_keysinit) {
    initKeys();
  }
  return &s_oiddelkey;
}

string utils::getUserName(context::SysFlowContext *cxt, uint32_t uid) {
  scap_userinfo *user = cxt->getInspector()->get_user(uid);
  if (user != nullptr) {
    return user->name;
  } else {
    return string("");
  }
}

string utils::getGroupName(context::SysFlowContext *cxt, uint32_t gid) {
  unordered_map<uint32_t, scap_groupinfo *>::const_iterator it;
  if (gid == 0xffffffff) {
    return string("");
  }

  it = cxt->getInspector()->m_grouplist.find(gid);
  if (it == cxt->getInspector()->m_grouplist.end()) {
    return string("");
  }

  return it->second->name;
}
bool utils::isInContainer(sinsp_evt *ev) {
  sinsp_threadinfo *ti = ev->get_thread_info();
  return !ti->m_container_id.empty();
}

time_t utils::getExportTime(context::SysFlowContext *cxt) {
  time_t now = utils::getCurrentTime(cxt);
  struct tm exportTM = *localtime(&now);
  exportTM.tm_sec += cxt->getNFExportInterval(); // add 30 seconds to the time
  return mktime(&exportTM);                      // normalize iti
}

int64_t utils::getSyscallResult(sinsp_evt *ev) {
  int64_t res = -1;
  if (ev->get_num_params() >= 1) {
    const ppm_param_info *param = ev->get_param_info(0);
    const sinsp_evt_param *p = ev->get_param(0);
    switch (param->type) {
    case PT_PID:
    case PT_ERRNO:
    case PT_FD:
    case PT_INT64:
    case PT_INT32:
      res = *reinterpret_cast<int64_t *>(p->m_val);
      break;
    default:
      SF_WARN(m_logger, "Syscall result not of type pid!! Type: "
                            << param->type << " Name: " << param->name);
      break;
    }
  }
  return res;
}

int64_t utils::getFlags(sinsp_evt *ev) {
  return utils::getIntParam(ev, "flags");
}

int64_t utils::getFD(sinsp_evt *ev) {
  return utils::getIntParam(ev, "fd");
}

bool utils::isMapAnonymous(sinsp_evt *ev) {
  int64_t flags = utils::getFlags(ev);
  return  flags & PPM_MAP_ANONYMOUS;
}

int64_t utils::getIntParam(sinsp_evt* ev, string pname) {
  uint32_t n = ev->get_num_params();
  for(uint32_t i = n; i >= 0; i--)
  {
    string name = ev->get_param_name(i);	  
    if(name.compare(pname) == 0)
    {
      const ppm_param_info *param = ev->get_param_info(i);
      switch (param->type) {
        case PT_PID:
        case PT_ERRNO:
        case PT_FD:
        case PT_INT64:
        case PT_INT32:
	case PT_FLAGS8:
	case PT_FLAGS16:
	case PT_FLAGS32:
		{
          const sinsp_evt_param *p = ev->get_param(i);
          return *reinterpret_cast<int64_t *>(p->m_val);
		}
        default:
	  return 0;
      }
    }
  }
  return 0;
}


bool utils::isCloneThreadSet(sinsp_evt *ev) {
  int64_t cloneThread = utils::getFlags(ev);
  return cloneThread & PPM_CL_CLONE_THREAD;
}

avro::ValidSchema utils::loadSchema(const char *filename) {
  avro::ValidSchema result;
  try {
    std::ifstream ifs(filename);
    avro::compileJsonSchema(ifs, result);
  } catch (avro::Exception &ex) {
    SF_ERROR(m_logger, "Unable to load schema file from "
                           << filename << " Error: " << ex.what());
    throw;
  }
  return result;
}

string utils::getPath(sinsp_evt *ev, const string &paraName) {
  int numParams = ev->get_num_params();
  string path;
  for (int i = 0; i < numParams; i++) {
    const ppm_param_info *param = ev->get_param_info(i);
    string name = ev->get_param_name(i);
    if (paraName.compare(name) != 0) {
      continue;
    }
    const sinsp_evt_param *p = ev->get_param(i);
    if (param->type == PT_FSPATH || param->type == PT_CHARBUF) {
      path = string(p->m_val, p->m_len);
      sanitize_string(path);
    }
    break;
  }
  return path;
}

int64_t utils::getFD(sinsp_evt *ev, const string &paraName) {
  int numParams = ev->get_num_params();
  int64_t fd = -1;
  for (int i = 0; i < numParams; i++) {
    const ppm_param_info *param = ev->get_param_info(i);
    string name = ev->get_param_name(i);
    if (paraName.compare(name) != 0) {
      continue;
    }
    const sinsp_evt_param *p = ev->get_param(i);
    if (param->type == PT_FD) {
      assert(p->m_len == sizeof(int64_t));
      fd = (*reinterpret_cast<int64_t *>(p->m_val));
    }
    break;
  }
  return fd;
}

fs::path utils::getCanonicalPath(const string &fileName) {
  fs::path p(fileName);
  try {
    p = fs::weakly_canonical(p);
  } catch (...) {
    SF_WARN(m_logger, "Unable to compute canonical path from " << fileName);
  }
  return p;
}

string utils::getAbsolutePath(sinsp_threadinfo *ti, int64_t dirfd,
                              const string &fileName) {
  fs::path p(fileName);
  SF_DEBUG(m_logger, "getAbsolutePath: The path is "
                         << p << " File name " << fileName << " is relative? "
                         << p.is_relative());
  if (fileName.empty() || p.is_relative()) {
    fs::path tmp;
    string cwd = ti->get_cwd();
    if (dirfd == PPM_AT_FDCWD) {
      if (cwd.empty()) {
        return p.string();
      }
      tmp = ti->get_cwd();
    } else {
      sinsp_fdinfo_t *fdinfo = ti->get_fd(dirfd);
      if (fdinfo == nullptr) {
        return p.string();
      }
      tmp = fdinfo->m_name;
      SF_DEBUG(m_logger,
               "getAbsolutePath: Retrieve fdinfo for fd. Path:  " << tmp);
    }
    tmp /= fileName;
    SF_DEBUG(m_logger, "getAbsolutePath: Before canonicalization: " << tmp);
    p = utils::getCanonicalPath(tmp);
    SF_DEBUG(m_logger, "getAbsolutePath: The canonicalized file is " << p);
  } else {
    p = utils::getCanonicalPath(p);
  }

  return p.string();
}

string utils::getAbsolutePath(sinsp_threadinfo *ti, const string &fileName) {
  fs::path p(fileName);
  SF_DEBUG(m_logger, "getAbsolutePath: The path is "
                         << p << " File name " << fileName << " is relative? "
                         << p.is_relative());
  if (fileName.empty() || p.is_relative()) {
    fs::path tmp;
    string cwd = ti->get_cwd();
    if (!cwd.empty()) {
      tmp = cwd;
      tmp /= fileName;
      SF_DEBUG(m_logger, "getAbsolutePath: Before canonicalization: " << tmp);
      p = utils::getCanonicalPath(tmp);
      SF_DEBUG(m_logger, "getAbsolutePath: The canonicalized file is " << p);
    }
  } else {
    p = utils::getCanonicalPath(p);
  }
  return p.string();
}

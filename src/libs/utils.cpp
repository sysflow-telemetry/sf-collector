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
#include "sysflow/avsc_sysflow5.hh"
#include "sysflowcontext.h"
#include "sha1.h"

static NFKey s_nfdelkey;
static NFKey s_nfemptykey;
static bool s_keysinit = false;
static OID s_oiddelkey;
static OID s_oidemptykey;

CREATE_LOGGER_2("sysflow.utils");

namespace {

const size_t CONTAINER_ID_LENGTH = 64;

/**
 * @brief A pattern to match cgroup paths against
 */
struct cgroup_layout {
  const char *prefix;
  const char *suffix;
};

/**
 * @brief Aggregated cgroup layout containing all known container runtime patterns
 */
constexpr const cgroup_layout ALL_RUNC_CGROUP_LAYOUTS[] = {
  // CRI patterns
  {"/crio-", ""},                  // non-systemd cri-o
  {"/cri-containerd-", ".scope"},  // systemd containerd
  {"/crio-", ".scope"},            // systemd cri-o
  {":cri-containerd:", ""},        // containerd without "SystemdCgroup = true"
  {"/docker-", ".scope"},          // systemd docker in cri-dockerd scenario
  // Podman patterns
  {"/libpod-", ".scope"},            // podman
  {"/libpod-", ".scope/container"},  // podman
  {"/libpod-", ""},                  // non-systemd podman, e.g. on alpine
  {nullptr, nullptr}
};

/**
 * Check if cgroup ends with <prefix><container_id><suffix>.
 */
static bool match_one_container_id(const std::string &cgroup,
                                   const std::string &prefix,
                                   const std::string &suffix) {
  // Single pass: find suffix first (from end), then validate prefix position
  size_t end_pos = cgroup.rfind(suffix);

  // Isn't this supposed to always be the last character?
  if(end_pos == std::string::npos) {
    return false;
  }

  // Calculate expected start position and validate prefix
  if(end_pos < prefix.size()) {
    return false;
  }
  size_t start_pos = cgroup.rfind(prefix, end_pos - 1);
  if(start_pos == std::string::npos) {
    return false;
  }
  start_pos += prefix.size();

  // Fast character validation using lookup instead of find_first_not_of
  if(end_pos - start_pos == CONTAINER_ID_LENGTH) {
    bool all_valid = true;
    for(size_t i = start_pos; i < end_pos && all_valid; ++i) {
      unsigned char c = cgroup[i];
      all_valid = (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
    }
    if(all_valid) {
      return true;
    }
  }

  return false;
}

static bool match_container_id(const std::string &cgroup,
                               const cgroup_layout *layout) {
  for(size_t i = 0; layout[i].prefix && layout[i].suffix; ++i) {
    if(match_one_container_id(cgroup, layout[i].prefix, layout[i].suffix)) {
      return true;
    }
  }

  return false;
}

static bool matches_runc_cgroups(const sinsp_threadinfo *tinfo,
                                 const cgroup_layout *layout) {
  for(const auto &it : tinfo->cgroups()) {
    if(match_container_id(it.second, layout)) {
      return true;
    }
  }

  return false;
}

}  // anonymous namespace

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

void utils::generateFOID(const std::string &key, FOID *foid) {
  SHA1 sha1;
  sha1.add(reinterpret_cast<const uint8_t*>(key.c_str()), key.size());
  sha1.getHash(foid->data());
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

// when querying users on the host, using an empty string for container id.
std::string utils::getUserName(context::SysFlowContext *cxt,
                               std::string &containerid, uint32_t uid) {
  scap_userinfo *user =
      cxt->getInspector()->m_usergroup_manager->get_user(containerid, uid);
  if (user != nullptr) {
    return user->name;
  } else {
    return EMPTY_STR;
  }
}

// when querying groups on the host, using an empty string for container id.
std::string utils::getGroupName(context::SysFlowContext *cxt,
                                std::string &containerid, uint32_t gid) {
  scap_groupinfo *group =
      cxt->getInspector()->m_usergroup_manager->get_group(containerid, gid);
  if (group != nullptr) {
    return group->name;
  } else {
    return EMPTY_STR;
  }
}

bool utils::isInContainer(sinsp_evt *ev) {
  sinsp_threadinfo *ti = ev->get_thread_info();
  if (ti == NULL) {
    return false;
  }

  // Check against all known container runtime patterns
  return matches_runc_cgroups(ti, ALL_RUNC_CGROUP_LAYOUTS);
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
      res = *reinterpret_cast<const int64_t *>(p->data());
      break;
    default:
      SF_DEBUG(m_logger, "Syscall result not of type pid! Type: "
                             << param->type << " Name: " << param->name);
      break;
    }
  }
  return res;
}

int64_t utils::getFlags(sinsp_evt *ev) {
  return utils::getIntParam(ev, "flags");
}

int64_t utils::getFD(sinsp_evt *ev) { return utils::getIntParam(ev, "fd"); }

bool utils::isMapAnonymous(sinsp_evt *ev) {
  int64_t flags = utils::getFlags(ev);
  return flags & PPM_MAP_ANONYMOUS;
}

int64_t utils::getIntParam(sinsp_evt *ev, std::string pname) {
  uint32_t n = ev->get_num_params();
  if (n == 0) {
    return -1;
  }
  for (uint32_t i = n - 1; i >= 0; i--) {
    std::string name = ev->get_param_name(i);
    if (name.compare(pname) == 0) {
      const ppm_param_info *param = ev->get_param_info(i);
      switch (param->type) {
      case PT_PID:
      case PT_ERRNO:
      case PT_FD:
      case PT_INT64:
      case PT_INT32:
      case PT_FLAGS8:
      case PT_FLAGS16:
      case PT_FLAGS32: {
        const sinsp_evt_param *p = ev->get_param(i);
        return *reinterpret_cast<const int64_t *>(p->data());
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

avro::ValidSchema utils::loadSchema() {
  avro::ValidSchema result;
  try {
    std::stringstream ss;
    ss << AVSC_SF;
    avro::compileJsonSchema(ss, result);
  } catch (avro::Exception &ex) {
    SF_ERROR(m_logger,
             "Unable to load avro sysflow schema Error: " << ex.what());
    throw;
  }
  return result;
}

int64_t utils::getSchemaVersion() {
  avro::ValidSchema result = utils::loadSchema();
  std::string strJson = result.toJson(true);
  Json::Value root;
  Json::Reader reader;
  bool succ = reader.parse(strJson.c_str(), root); // parse process
  if (!succ) {
    SF_ERROR(m_logger, "Unable to parse avro sysflow schema Error: "
                           << reader.getFormattedErrorMessages());
    return -1;
  }
  if (root.isMember(SCH_FIELDS_STR) && root[SCH_FIELDS_STR].size() > 0 &&
      root[SCH_FIELDS_STR][0].isMember(SCH_TYPE_STR)) {
    const Json::Value fields = root[SCH_FIELDS_STR][0][SCH_TYPE_STR];
    for (unsigned int i = 0; i < fields.size(); i++) {
      const Json::Value obj = fields[i];
      if (obj.isMember(SCH_NAME_STR) && obj[SCH_NAME_STR].isString() &&
          obj[SCH_NAME_STR].asString().compare(SCH_SFHEADER_STR) == 0) {
        if (obj.isMember(SCH_FIELDS_STR)) {
          const Json::Value f = obj[SCH_FIELDS_STR];
          for (unsigned int j = 0; j < f.size(); j++) {
            if (f[j].isMember(SCH_NAME_STR) && f[j][SCH_NAME_STR].isString() &&
                f[j][SCH_NAME_STR].asString().compare(SCH_VERSION_STR) == 0) {
              if (f[j].isMember(SCH_DEFAULT_STR) &&
                  f[j][SCH_DEFAULT_STR].isInt64()) {
                int64_t version = f[j][SCH_DEFAULT_STR].asInt64();
                return version;
              }
            }
          }
        }
        break;
      }
    }
  }

  SF_ERROR(m_logger, "Unable to find schema version in avro schema.")
  return -1;
}

std::string utils::getPath(sinsp_evt *ev, const std::string &paraName) {
  int numParams = ev->get_num_params();
  std::string path;
  for (int i = 0; i < numParams; i++) {
    const ppm_param_info *param = ev->get_param_info(i);
    std::string name = ev->get_param_name(i);
    if (paraName.compare(name) != 0) {
      continue;
    }
    SF_DEBUG(m_logger,
             "getPath: Found '" << name << "' of type " << param->type);
    const sinsp_evt_param *p = ev->get_param(i);
    if (param->type == PT_FSPATH || param->type == PT_CHARBUF ||
        param->type == PT_FSRELPATH) {
      path = std::string(p->data(), p->len());
      SF_DEBUG(m_logger, "getPath: Param '" << name << "'s value is " << path);
      sanitize_string(path);
    }
    break;
  }
  return path;
}

int64_t utils::getFD(sinsp_evt *ev, const std::string &paraName) {
  int numParams = ev->get_num_params();
  int64_t fd = -1;
  for (int i = 0; i < numParams; i++) {
    const ppm_param_info *param = ev->get_param_info(i);
    std::string name = ev->get_param_name(i);
    if (paraName.compare(name) != 0) {
      continue;
    }
    const sinsp_evt_param *p = ev->get_param(i);
    if (param->type == PT_FD) {
      assert(p->len() == sizeof(int64_t));
      fd = (*reinterpret_cast<const int64_t *>(p->data()));
    }
    break;
  }
  return fd;
}

fs::path utils::getCanonicalPath(const std::string &fileName) {
  fs::path p(fileName);
  try {
    p = fs::weakly_canonical(p);
  } catch (...) {
    SF_DEBUG(m_logger, "Unable to compute canonical path from " << fileName);
  }
  return p;
}

std::string utils::getAbsolutePath(sinsp_threadinfo *ti, int64_t dirfd,
                                   const std::string &fileName) {
  fs::path p(fileName);
  SF_DEBUG(m_logger, "getAbsolutePath: The path is "
                         << p << " File name " << fileName << " is relative? "
                         << p.is_relative());
  if (fileName.empty() || p.is_relative()) {
    fs::path tmp;
    std::string cwd = ti->get_cwd();
    if (dirfd == PPM_AT_FDCWD) {
      if (cwd.empty()) {
        return p.string();
      }
      tmp = ti->get_cwd();
    } else {
      sinsp_fdinfo *fdinfo = ti->get_fd(dirfd);
      if (fdinfo == nullptr) {
        return p.string();
      }
      tmp = fdinfo->m_name;
      SF_DEBUG(m_logger,
               "getAbsolutePath: Retrieve fdinfo for fd. Path:  " << tmp);
    }
    tmp /= p;
    SF_DEBUG(m_logger, "getAbsolutePath: Before canonicalization: " << tmp);
    p = utils::getCanonicalPath(tmp);
    SF_DEBUG(m_logger, "getAbsolutePath: The canonicalized file is " << p);
  } else {
    p = utils::getCanonicalPath(p);
  }

  return p.string();
}

std::string utils::getAbsolutePath(sinsp_threadinfo *ti,
                                   const std::string &fileName) {
  fs::path p(fileName);
  SF_DEBUG(m_logger, "getAbsolutePath: The path is "
                         << p << " File name " << fileName << " is relative? "
                         << p.is_relative());
  if (fileName.empty() || p.is_relative()) {
    fs::path tmp;
    std::string cwd = ti->get_cwd();
    if (!cwd.empty()) {
      tmp = cwd;
      tmp /= p;
      SF_DEBUG(m_logger, "getAbsolutePath: Before canonicalization: " << tmp);
      p = utils::getCanonicalPath(tmp);
      SF_DEBUG(m_logger, "getAbsolutePath: The canonicalized file is " << p);
    }
  } else {
    p = utils::getCanonicalPath(p);
  }
  return p.string();
}
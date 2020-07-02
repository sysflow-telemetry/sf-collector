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

#include "sysdig/sdfiledescinfo.h"

using sysdig::SDFileDescInfo;

SDFileDescInfo::SDFileDescInfo() : fdinfo(nullptr), ev(nullptr) {}

SDFileDescInfo::~SDFileDescInfo() {}

int32_t SDFileDescInfo::getOpenFlag() { return fdinfo->m_openflags; }

bool SDFileDescInfo::isIPSocket() {
  return (fdinfo->is_ipv4_socket() || fdinfo->is_ipv6_socket());
}

bool SDFileDescInfo::isIPv6Socket() { return fdinfo->is_ipv6_socket(); }

char SDFileDescInfo::getFileType() { return fdinfo->get_typechar(); }

std::string SDFileDescInfo::getName() { return fdinfo->m_name; }

uint32_t SDFileDescInfo::getSIPv4() {
  return fdinfo->m_sockinfo.m_ipv4info.m_fields.m_sip;
}

uint32_t SDFileDescInfo::getDIPv4() {
  return fdinfo->m_sockinfo.m_ipv4info.m_fields.m_dip;
}

uint32_t SDFileDescInfo::getSPort() {
  return fdinfo->m_sockinfo.m_ipv4info.m_fields.m_sport;
}

uint32_t SDFileDescInfo::getDPort() {
  return fdinfo->m_sockinfo.m_ipv4info.m_fields.m_dport;
}

int32_t SDFileDescInfo::getProtocol() {
  int32_t prt = -1;
  switch (fdinfo->get_l4proto()) {
  case SCAP_L4_TCP:
    prt = 6;
    break;
  case SCAP_L4_UDP:
    prt = 17;
    break;
  case SCAP_L4_ICMP:
    prt = 1;
    break;
  case SCAP_L4_RAW:
    prt = 254;
    break;
  default:
    break;
  }
  return prt;
}

void SDFileDescInfo::printIPTupleDebug() {
  std::string ip4tuple =
      ipv4tuple_to_string(&(fdinfo->m_sockinfo.m_ipv4info), false);
  SF_DEBUG(m_logger,
           ev->get_thread_info()->m_exepath
               << " " << ip4tuple << " Proto: " << getProtocol()
               << " Server: " << fdinfo->is_role_server() << " Client: "
               << fdinfo->is_role_client() << " " << ev->get_name() << " "
               << ev->get_thread_info()->m_tid << " " << ev->get_fd_num());
}

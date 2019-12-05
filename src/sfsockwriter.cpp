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

#include "sfsockwriter.h"

using writer::SFSocketWriter;

CREATE_LOGGER(SFSocketWriter, "sysflow.sfsocketwriter");

SFSocketWriter::SFSocketWriter(context::SysFlowContext *cxt, time_t start)
    : writer::SysFlowWriter(cxt, start), m_sock(0) {
  m_sockPath = m_cxt->getOutputFile();
}

SFSocketWriter::~SFSocketWriter() { close(m_sock); }

int SFSocketWriter::initialize() {
  m_outStream = avro::ostreamOutputStream(m_stringStream, 16);
  m_encoder = avro::binaryEncoder();
  m_encoder->init(*m_outStream);
  struct sockaddr_un addr;
  if ((m_sock = socket(AF_UNIX, SOCK_SEQPACKET, 0)) < 0) {
    SF_ERROR(m_logger, "Unable to create domain socket object. Error Code: "
                           << std::strerror(errno));
    exit(EXIT_FAILURE);
  }
  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, m_sockPath.c_str(), sizeof(addr.sun_path) - 1);
  if (connect(m_sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    SF_ERROR(m_logger, "Unable to connect to domain socket: "
                           << m_sockPath
                           << ". Error Code: " << std::strerror(errno));
    exit(EXIT_FAILURE);
  }
  writeHeader();
  return 0;
}

void SFSocketWriter::reset(time_t curTime) {
  m_numRecs = 0;
  m_start = curTime;
  writeHeader();
}

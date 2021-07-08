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

#include "sysflowwriter.h"

using writer::SysFlowWriter;

SysFlowWriter::SysFlowWriter(context::SysFlowContext *cxt, time_t start)
    : m_start(start), m_version(0), m_hdrFile("") {
  m_cxt = cxt;
  m_start = start;
  m_version = utils::getSchemaVersion();
}

void SysFlowWriter::writeHeader() {
  sysflow::SFHeader header;
  header.version = m_version;
  header.exporter = m_cxt->getExporterID();
  header.ip = m_cxt->getNodeIP();
  header.filename = m_hdrFile;
  m_flow.rec.set_SFHeader(header);
  m_numRecs++;
  write(&m_flow);
}

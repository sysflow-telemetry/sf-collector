/** Copyright (C) 2021 IBM Corporation.
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

#include "sfmultiwriter.h"

using writer::SFMultiWriter;

SFMultiWriter::SFMultiWriter(context::SysFlowContext *cxt, time_t start)
    : writer::SysFlowWriter(cxt, start), m_sockWriter(cxt, start),
      m_fileWriter(cxt, start) {}

SFMultiWriter::~SFMultiWriter() {}

int SFMultiWriter::initialize() {
  m_fileWriter.initialize();
  m_sockWriter.setHeaderFile(m_fileWriter.getHeaderFile());
  m_sockWriter.initialize();
  return 0;
}

void SFMultiWriter::reset(time_t curTime) {
  m_fileWriter.reset(curTime);
  m_sockWriter.setHeaderFile(m_fileWriter.getHeaderFile());
  m_sockWriter.reset(curTime);
  m_start = curTime;
}

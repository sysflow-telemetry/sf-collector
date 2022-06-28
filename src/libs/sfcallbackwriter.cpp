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

#include "sfcallbackwriter.h"
#include "sysflowprocessor.h"

using writer::SFCallbackWriter;

SFCallbackWriter::SFCallbackWriter(context::SysFlowContext *cxt, time_t start, SysFlowCallback callback, sysflowprocessor::SysFlowProcessor* proc)
    : writer::SysFlowWriter(cxt, start) {
      m_callback = callback;
      m_sysflowProc = proc;
}

SFCallbackWriter::~SFCallbackWriter() {
}

int SFCallbackWriter::initialize() {
  writeHeader();
  return 0;
}

void SFCallbackWriter::reset(time_t curTime) {
  writeHeader();
}

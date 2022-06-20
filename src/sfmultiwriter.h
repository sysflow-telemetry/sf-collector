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

#ifndef __SF_MULTI_WRITER_
#define __SF_MULTI_WRITER_
#include "sffilewriter.h"
#include "sfsockwriter.h"
#include "sysflowcontext.h"
#include "sysflowwriter.h"
using sysflow::SysFlow;

namespace writer {
class SFMultiWriter : public writer::SysFlowWriter {
private:
  SFSocketWriter m_sockWriter;
  SFFileWriter m_fileWriter;
  DEFINE_LOGGER();

public:
  SFMultiWriter(context::SysFlowContext *cxt, time_t start);
  virtual ~SFMultiWriter();
  inline void write(SysFlow *flow, sysflow::Process*, sysflow::File*, sysflow::File*) {
    write(flow);
  }
  inline void write(SysFlow *flow) {
    m_sockWriter.write(flow);
    m_fileWriter.write(flow);
  }
  int initialize();
  void reset(time_t curTime);
  bool needsReset() {
    return m_sockWriter.needsReset() || m_fileWriter.needsReset();
  }
};
} // namespace writer
#endif

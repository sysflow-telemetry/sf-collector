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

#ifndef __SF_FILE_WRITER_
#define __SF_FILE_WRITER_
#include "avro/Compiler.hh"
#include "avro/DataFile.hh"
#include "avro/Decoder.hh"
#include "avro/Encoder.hh"
#include "avro/ValidSchema.hh"
#include "sysflow.h"
#include "sysflowcontext.h"
#include "sysflowwriter.h"
#include "utils.h"
#define COMPRESS_BLOCK_SIZE 80000

using sysflow::SysFlow;

namespace writer {
class SFFileWriter : public writer::SysFlowWriter {
private:
  avro::ValidSchema m_sysfSchema;
  avro::DataFileWriter<SysFlow> *m_dfw;
  string getFileName(time_t curTime);

public:
  SFFileWriter(context::SysFlowContext *cxt, time_t start);
  virtual ~SFFileWriter();
  inline void write(SysFlow *flow) { m_dfw->write(*flow); }
  int initialize();
  void reset(time_t curTime);
};
} // namespace writer
#endif

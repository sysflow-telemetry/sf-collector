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

#include "sffilewriter.h"

using writer::SFFileWriter;

SFFileWriter::SFFileWriter(context::SysFlowContext *cxt, time_t start) : writer::SysFlowWriter(cxt, start), m_dfw(nullptr) {
  m_sysfSchema = utils::loadSchema(m_cxt->getSchemaFile());
}

SFFileWriter::~SFFileWriter() {
  if (m_dfw != nullptr) {
    m_dfw->close();
    delete m_dfw;
  }
}

int SFFileWriter::initialize() {
  time_t curTime = time(nullptr);
  string ofile = getFileName(curTime);
  m_dfw = new avro::DataFileWriter<SysFlow>(ofile.c_str(), m_sysfSchema,
                                            COMPRESS_BLOCK_SIZE,
                                            avro::Codec::DEFLATE_CODEC);
  writeHeader();
  return 0;
}

string SFFileWriter::getFileName(time_t curTime) {
  string ofile;
  if (m_start > 0) {
    if (m_cxt->hasPrefix()) {
      ofile = m_cxt->getOutputFile() + "." + std::to_string(curTime);
    } else {
      ofile = m_cxt->getOutputFile() + std::to_string(curTime);
    }
  } else {
    if (m_cxt->hasPrefix()) {
      ofile = m_cxt->getOutputFile();
    } else {
      ofile = m_cxt->getOutputFile() + std::to_string(curTime);
    }
  }
  return ofile;
}

void SFFileWriter::reset(time_t curTime) {
  string ofile = getFileName(curTime);
  m_numRecs = 0;
  m_dfw->close();
  delete m_dfw;
  m_dfw = new avro::DataFileWriter<SysFlow>(ofile.c_str(), m_sysfSchema,
                                            COMPRESS_BLOCK_SIZE,
                                            avro::Codec::DEFLATE_CODEC);
  m_start = curTime;
  writeHeader();
}

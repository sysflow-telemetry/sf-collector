/** Copyright (C) 2019 IBM Corporation.
*
* Authors:
* Teryl Taylor <terylt@ibm.com>
* Frederico Araujo <frederico.araujo@ibm.com>
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

SysFlowWriter::SysFlowWriter(SysFlowContext* cxt, time_t start) : m_dfw(NULL), m_start(start) {
   m_cxt = cxt;
   m_sysfSchema  = utils::loadSchema(m_cxt->getSchemaFile());
   m_start = start;
}


SysFlowWriter::~SysFlowWriter() {
   if(m_dfw != NULL) {
      m_dfw->close();
      delete m_dfw;
   }
}

void SysFlowWriter::writeHeader() {
   SFHeader header;
   header.version = 1000;
  /* const scap_machine_info* mi = inspector->get_machine_info();
   if(mi != NULL) {
       header.exporter = mi->hostname;
   }*/
   header.exporter = m_cxt->getExporterID();
   m_flow.rec.set_SFHeader(header);
   m_numRecs++;
   m_dfw->write(m_flow);
}

int SysFlowWriter::initialize() {
    time_t curTime = time(NULL);
    string ofile = getFileName(curTime);
    m_dfw = new avro::DataFileWriter<SysFlow>(ofile.c_str(), m_sysfSchema, COMPRESS_BLOCK_SIZE, avro::Codec::DEFLATE_CODEC); 
    writeHeader();
    return 0;
}

string SysFlowWriter::getFileName(time_t curTime) {
    string ofile;
    if(m_start > 0) {
       if(m_cxt->hasPrefix()) {
           ofile = m_cxt->getOutputFile() + "." + std::to_string(curTime);
       } else {
           ofile = m_cxt->getOutputFile() + std::to_string(curTime);
       }
    } else {
        if(m_cxt->hasPrefix()) {
            ofile = m_cxt->getOutputFile();
        } else {
           ofile = m_cxt->getOutputFile() + std::to_string(curTime);
        }
    }
    return ofile;
}

void SysFlowWriter::resetFileWriter(time_t curTime) {
    string ofile = getFileName(curTime);
    m_numRecs = 0;
    m_dfw->close();
    delete m_dfw; 
    m_dfw = new avro::DataFileWriter<SysFlow>(ofile.c_str(), m_sysfSchema, COMPRESS_BLOCK_SIZE, avro::Codec::DEFLATE_CODEC); 
    m_start = curTime;
    writeHeader();
}

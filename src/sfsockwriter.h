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

#ifndef __SF_SOCK_WRITER_
#define __SF_SOCK_WRITER_
#include "avro/Decoder.hh"
#include "avro/Encoder.hh"
#include "sysflow.h"
#include "sysflowcontext.h"
#include "sysflowwriter.h"
#include "utils.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sstream>

using sysflow::SysFlow;

namespace writer {
class SFSocketWriter : public writer::SysFlowWriter {
private:
  int m_sock;
  string m_sockPath;
  avro::EncoderPtr m_encoder;
  std::ostringstream m_stringStream;
  std::unique_ptr<avro::OutputStream> m_outStream;
  DEFINE_LOGGER();

public:
  SFSocketWriter(context::SysFlowContext *cxt, time_t start);
  virtual ~SFSocketWriter();
  inline void write(SysFlow* flow) {
    avro::encode(*m_encoder, *flow);
    m_encoder->flush();
    if(send (m_sock, (const void*)m_stringStream.str().c_str(), m_stringStream.str().size(), 0) <0) {
      SF_ERROR(m_logger,
                 "Unable to send on domain socket:  " << m_sockPath << ". Error Code: "
                 << std::strerror(errno));
    }

    m_stringStream.str("");
    m_stringStream.clear();
  }
  int initialize();
  void reset(time_t curTime);
};
} // namespace writer
#endif

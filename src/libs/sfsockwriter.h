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
#include <netinet/in.h>
#include <signal.h>
#include <sstream>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

using sysflow::SysFlow;

#define CONNECT_INTERVAL 10

namespace writer {
class SFSocketWriter : public writer::SysFlowWriter {
private:
  int m_sock;
  std::string m_sockPath;
  avro::EncoderPtr m_encoder;
  std::ostringstream m_stringStream;
  std::unique_ptr<avro::OutputStream> m_outStream;
  time_t m_errTimer;
  time_t m_reconnectInterval;
  bool m_reset;
  DEFINE_LOGGER();
  int connectSocket();

public:
  SFSocketWriter(context::SysFlowContext *cxt, time_t start);
  virtual ~SFSocketWriter();

  inline void write(SysFlow *flow, sysflow::Process *, sysflow::File *,
                    sysflow::File *) {
    write(flow);
  }

  inline void write(SysFlow *flow) {
    if (m_errTimer == 0) {
      avro::encode(*m_encoder, *flow);
      m_encoder->flush();
      if (send(m_sock, (const void *)m_stringStream.str().c_str(),
               m_stringStream.str().size(), 0) < 0) {
        SF_ERROR(m_logger, "Unable to send on domain socket:  "
                               << m_sockPath
                               << ". Error Code: " << std::strerror(errno));
        m_errTimer = time(nullptr);
      }
      m_stringStream.str("");
      m_stringStream.clear();
    } else {
      time_t curTime = time(nullptr);
      double interval = difftime(curTime, m_errTimer);
      if (interval >= m_reconnectInterval) {
        SF_WARN(m_logger,
                "Trying to reconnect to socket " << m_sockPath.c_str())
        int res = connectSocket();
        if (res == 0) {
          SF_WARN(m_logger,
                  "Successfully reconnected to socket " << m_sockPath.c_str())
          m_errTimer = 0;
          m_reconnectInterval = CONNECT_INTERVAL;
          m_reset = true;
        } else {
          m_reconnectInterval = 2 * m_reconnectInterval;
          if (m_reconnectInterval > 8 * CONNECT_INTERVAL) {
            SF_ERROR(
                m_logger,
                "Unable to connect to domain socket within interval. Exiting!");
            pid_t myPid = getpid();
            kill(myPid, SIGINT);
          }
        }
      }
    }
  }
  int initialize();
  void reset(time_t curTime);
  bool needsReset() { return m_reset; }
};
} // namespace writer
#endif

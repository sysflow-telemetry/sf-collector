/** Copyright (C) 2022 IBM Corporation.
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

#ifndef __SYSFLOW_LIBS_C_PLUS_PLUS_API__
#define __SYSFLOW_LIBS_C_PLUS_PLUS_API__
#include "logger.h"
#include "sysflowcontext.h"
#include "sysflowprocessor.h"
#include <string>

namespace sysflowlibscpp {

SysFlowConfig *InitializeSysFlowConfig();

class SysFlowDriver {
private:
  DEFINE_LOGGER();
  context::SysFlowContext *m_cxt;
  writer::SysFlowWriter *m_writer;
  sysflowprocessor::SysFlowProcessor *m_processor;

public:
  explicit SysFlowDriver(SysFlowConfig *conf);
  virtual ~SysFlowDriver();
  inline void exit() { m_processor->exit(); }
  inline int run() { return m_processor->run(); }
};

} // namespace sysflowlibscpp

#endif // __SYSFLOW_LIBS_C_PLUS_PLUS_API__

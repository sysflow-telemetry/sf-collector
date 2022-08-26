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

#include "sysflowlibs.hpp"
#include "sysflowcontext.h"
#include "sysflowprocessor.h"

using sysflowlibscpp::SysFlowDriver;

SysFlowDriver::SysFlowDriver(SysFlowConfig *conf) {
  m_cxt = new context::SysFlowContext(conf);
  m_processor = new sysflowprocessor::SysFlowProcessor(m_cxt, nullptr);
}

SysFlowDriver::~SysFlowDriver() { delete m_processor; }

SysFlowConfig *sysflowlibscpp::InitializeSysFlowConfig() {
  SysFlowConfig *conf = new SysFlowConfig();
  conf->filterContainers = false;
  conf->rotateInterval = 300;
  conf->samplingRatio = 1;
  conf->criTO = 30;
  conf->enableStats = false;
  conf->enableProcessFlow = true;
  conf->fileOnly = true;
  conf->fileReadMode = 2;
  conf->dropMode = true;
  conf->callback = nullptr;
  conf->debugMode = false;
  conf->enableConsumerMode = false;
  conf->singleBufferDimension = 0;
  return conf;
}

void DeleteSysFlowConfig(SysFlowConfig **conf) {
  delete *conf;
  *conf = nullptr;
}

void SysFlowDriver::exit() { m_processor->exit(); }

int SysFlowDriver::run() { return m_processor->run(); }

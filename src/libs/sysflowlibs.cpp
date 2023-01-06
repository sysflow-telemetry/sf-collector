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
#include "scap.h"
#include "scap_open_exception.h"
#include "sinsp_exception.h"
#include "sysflow_config.h"
#include "sysflowcontext.h"
#include "sysflowprocessor.h"
#include <sstream>

using sysflowlibscpp::SysFlowDriver;

SysFlowDriver::SysFlowDriver(SysFlowConfig *conf) {
  try {
    CONFIGURE_LOGGER(conf->appName.c_str())
    m_cxt = new context::SysFlowContext(conf);
    m_processor = new sysflowprocessor::SysFlowProcessor(m_cxt, nullptr);
  } catch (const sinsp_exception &ex) {
    SF_ERROR(logger, "Runtime exception on module load: " << ex.what());
    SHUTDOWN_LOGGER()
    throw sfexception::SysFlowException(ex.what());
  }
}

SysFlowDriver::~SysFlowDriver() {
  delete m_processor;
  SHUTDOWN_LOGGER()
}

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
  conf->moduleChecks = true;
  conf->singleBufferDimension = DEFAULT_DRIVER_BUFFER_BYTES_DIM;
  conf->appName = "sysflowlibs";
  conf->collectionMode = SFSysCallMode::SFFlowMode;
  return conf;
}

void SysFlowDriver::exit() { m_processor->exit(); }

int SysFlowDriver::run() {
  try {
    return m_processor->run();
  } catch (const sinsp_exception &ex) {
    SF_ERROR(logger, "Runtime exception caught in main loop: " << ex.what());
    throw sfexception::SysFlowException(ex.what());
  } catch (const avro::Exception &aex) {
    SF_ERROR(logger,
             "Runtime avro exception caught in main loop: " << aex.what());
    throw sfexception::SysFlowException(aex.what());
  }
  return 0;
}

std::string SysFlowDriver::getVersion() {
  std::stringstream str;
  str << SF_VERSION << "+" << SF_BUILD;
  return str.str();
}

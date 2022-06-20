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

using sysflowlibscpp::SysFlowDriver;

SysFlowDriver::SysFlowDriver(SysFlowConfig *conf) {
    m_cxt = new context::SysFlowContext(conf->filterContainers,
                                        conf->rotateInterval,
                                        std::string(conf->filePath),
                                        std::string(conf->socketPath),
                                        std::string(conf->scapInputPath),
                                        conf->samplingRatio,
                                        std::string(conf->exportID),
                                        std::string(conf->falcoFilter), 
                                        std::string(conf->criPath),
                                        conf->criTO);
    if(conf->enableStats) {
        m_cxt->enableStats();
    }
    m_cxt->setNodeIP(std::string(conf->nodeIP));
    if (conf->enableProcessFlow) {
        m_cxt->enableProcessFlow();
    }
    if (conf->fileOnly) {
        m_cxt->enableFileOnly();
    }
    if (conf->dropMode) {
        m_cxt->enableDropMode();
    }
    if (conf->debugMode) {
        m_cxt->enableDebugMode();
    }
    if (conf->callback != nullptr) {
        m_cxt->setCallback(conf->callback);
    }
    m_cxt->setReadFileMode(conf->fileReadMode);
}

SysFlowDriver::~SysFlowDriver() {

}

SysFlowConfig* sysflowlibscpp::InitializeSysFlowConfig() {
   SysFlowConfig* conf = new SysFlowConfig();
   conf->filterContainers = false;
   conf->rotateInterval = 300;
   memset(conf->exportID, 0, 256);
   memset(conf->nodeIP, 0, 256);
   memset(conf->filePath, 0, 256);
   memset(conf->socketPath, 0, 256);
   memset(conf->scapInputPath, 0, 256);
   memset(conf->falcoFilter, 0, 2048);
   conf->samplingRatio = 1;
   memset(conf->criPath, 0, 256);
   conf->criTO = 30;
   conf->enableStats = false;
   conf->enableProcessFlow = true;
   conf->fileOnly = true;
   conf->fileReadMode = 2;
   conf->dropMode = true;
   conf->callback = nullptr;
   conf->debugMode = false;
   return conf;
}

void DeleteSysFlowConfig(SysFlowConfig **conf) {
    delete *conf;
    *conf = nullptr;
}

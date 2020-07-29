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

#include "api/sfinspector.h"
#include "processcontext.h"
#include "filecontext.h"
#include "containercontext.h"

using api::SysFlowContainer;
using api::SysFlowEvent;
using api::SysFlowFileDescInfo;
using api::SysFlowInspector;
using api::SysFlowProcess;

SysFlowInspector::SysFlowInspector(context::SysFlowContext *cxt,
                                   process::ProcessContext *procCxt,
                                   file::FileContext *fileCxt,
                                   container::ContainerContext *contCxt) {
  m_cxt = cxt;
  m_procCxt = procCxt;
  m_fileCxt = fileCxt;
  m_contCxt = contCxt;
}

SysFlowInspector::~SysFlowInspector() {}

SysFlowEvent::~SysFlowEvent() {}

SysFlowFileDescInfo::~SysFlowFileDescInfo() {}

SysFlowContainer::~SysFlowContainer() {}

SysFlowProcess::~SysFlowProcess() {}

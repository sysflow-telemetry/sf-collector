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
#ifndef SF_INSPECTOR
#define SF_INSPECTOR
#include "op_flags.h"
#include "sysflow.h"
#include "sysflowcontext.h"
#include <memory>
#include <string>
#include <utility>

#define SF_SUCCESS 0
#define SF_ERR 1
#define SF_TIMEOUT -1
#define SF_EOF 6

namespace process {
class ProcessContext;
}

namespace container {
class ContainerContext;
}

namespace file {
class FileContext;
}

namespace api {
class SysFlowFileDescInfo {
public:
  virtual ~SysFlowFileDescInfo();
  virtual int32_t getOpenFlag() = 0;
  virtual bool isIPSocket() = 0;
  virtual bool isIPv6Socket() = 0;
  virtual char getFileType() = 0;
  virtual std::string getName() = 0;
  virtual uint32_t getSIPv4() = 0;
  virtual uint32_t getDIPv4() = 0;
  virtual uint32_t getSPort() = 0;
  virtual uint32_t getDPort() = 0;
  virtual int32_t getProtocol() = 0;
  virtual void printIPTupleDebug() = 0;
};

class SysFlowContainer {
public:
  virtual ~SysFlowContainer();
  virtual std::string getName() = 0;
  virtual std::string getImage() = 0;
  virtual std::string getImageTag() = 0;
  virtual std::string getID() = 0;
  virtual std::string getImageID() = 0;
  virtual sysflow::ContainerType getType() = 0;
  virtual bool getPriv() = 0;
};

class SysFlowProcess {
public:
  virtual ~SysFlowProcess();
  virtual uint64_t getCreateTS() = 0;
  virtual uint64_t getHostPID() = 0;
  virtual std::string getExe() = 0;
  virtual std::string getArgs() = 0;
  virtual int64_t getUID() = 0;
  virtual int64_t getGID() = 0;
  virtual int64_t getParentPID() = 0;
  virtual int64_t getParentCreateTS() = 0;
  virtual std::string getUserName() = 0;
  virtual std::string getGroupName() = 0;
  virtual bool isInContainer() = 0;
  virtual bool hasParent() = 0;
  virtual bool getTTY() = 0;
  virtual SysFlowContainer *getContainer() = 0;
  virtual std::string getContainerID() = 0;
};

class SysFlowEvent {
public:
  OpFlags opFlag;
  bool sysCallEnter;
  virtual ~SysFlowEvent();
  virtual bool isSysCallEnter() = 0;
  virtual int64_t getTS() = 0;
  virtual int64_t getTID() = 0;
  virtual int64_t getSysCallResult() = 0;
  virtual int64_t getFlags() = 0;
  virtual bool isMapAnonymous() = 0;
  virtual bool isCloneThreadSet() = 0;
  virtual bool isMainThread() = 0;
  virtual SysFlowProcess *getProcess() = 0;
  virtual bool isInContainer() = 0;
  virtual std::string getContainerID() = 0;
  virtual std::string getName() = 0;
  virtual std::string getCWD() = 0;
  virtual int64_t getFD() = 0;
  virtual std::string getUIDFromParameter() = 0;
  virtual api::SysFlowFileDescInfo *getFileDescInfo() = 0;
  virtual std::string getPathForFileEvent() = 0;
  virtual std::pair<std::string, std::string> getLinkPaths() = 0;
};

class SysFlowInspector {
protected:
  context::SysFlowContext *m_cxt;
  process::ProcessContext *m_procCxt;
  file::FileContext *m_fileCxt;
  container::ContainerContext *m_contCxt;

public:
  SysFlowInspector(context::SysFlowContext *cxt,
                   process::ProcessContext *procCxt, file::FileContext *fileCxt,
                   container::ContainerContext *contCxt);
  virtual ~SysFlowInspector();
  virtual int init() = 0;
  virtual int next(SysFlowEvent **evt) = 0;
  virtual int cleanup() = 0;
  virtual std::unique_ptr<SysFlowProcess> getProcess(int64_t pid) = 0;
  virtual std::string getLastError() = 0;
};

} // namespace api

#endif

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

#include "fileeventprocessor.h"
#include <cassert>

using fileevent::FileEventProcessor;

CREATE_LOGGER(FileEventProcessor, "sysflow.fileevent");

FileEventProcessor::FileEventProcessor(writer::SysFlowWriter *writer,
                                       process::ProcessContext *procCxt,
                                       file::FileContext *fileCxt) {
  m_writer = writer;
  m_processCxt = procCxt;
  m_fileCxt = fileCxt;
}

FileEventProcessor::~FileEventProcessor() = default;

int FileEventProcessor::handleFileFlowEvent(api::SysFlowEvent *ev) {
  int res = 1;
  OpFlags flag = ev->opFlag;
  if (flag == OP_MKDIR || flag == OP_RMDIR || flag == OP_UNLINK) {
    res = writeFileEvent(ev);
  } else if (flag == OP_LINK || flag == OP_SYMLINK || flag == OP_RENAME) {
    res = writeLinkEvent(ev);
  }

  return res;
}

int FileEventProcessor::writeLinkEvent(api::SysFlowEvent *ev) {
  bool created = false;
  ProcessObj *proc = m_processCxt->getProcess(ev, SFObjectState::REUP, created);
  OpFlags flag = ev->opFlag;
  FileObj *file1 = nullptr;
  FileObj *file2 = nullptr;
  
  auto paths = ev->getLinkPaths();
  SF_DEBUG(m_logger, "Path parameters for ev: " << ev->getName() << " are "
                                                << paths.first
                                                << " Path2: " << paths.second);

  file1 = m_fileCxt->getFile(ev, paths.first,  SF_UNK, SFObjectState::REUP, created);
  file2 =
      m_fileCxt->getFile(ev, paths.second, SF_UNK, SFObjectState::CREATED, created);

  m_fileEvt.opFlags = flag;
  m_fileEvt.ts = ev->getTS();
  m_fileEvt.procOID.hpid = proc->proc.oid.hpid;
  m_fileEvt.procOID.createTS = proc->proc.oid.createTS;
  m_fileEvt.tid = ev->getTID();
  m_fileEvt.ret = ev->getSysCallResult();
  m_fileEvt.fileOID = file1->file.oid;
  m_fileEvt.newFileOID.set_FOID(file2->file.oid);

  SF_DEBUG(m_logger, "The Current working Directory of the "
                         << ev->getName() << " event is " << ev->getCWD());
  m_writer->writeFileEvent(&m_fileEvt);
  return 0;
}

int FileEventProcessor::writeFileEvent(api::SysFlowEvent *ev) {
  OpFlags flag = ev->opFlag;
  bool created = false;
  ProcessObj *proc = m_processCxt->getProcess(ev, SFObjectState::REUP, created);
  api::SysFlowFileDescInfo *fdinfo = ev->getFileDescInfo();
  FileObj *file = nullptr;
  if (fdinfo != nullptr) {
    file = m_fileCxt->getFile(ev, fdinfo, SFObjectState::CREATED, created);
  } else {
    std::string fileName = ev->getPathForFileEvent();
    file = m_fileCxt->getFile(ev, fileName, SF_UNK, SFObjectState::CREATED,
                              created);
  }
  m_fileEvt.opFlags = flag;
  m_fileEvt.ts = ev->getTS();
  m_fileEvt.procOID.hpid = proc->proc.oid.hpid;
  m_fileEvt.procOID.createTS = proc->proc.oid.createTS;
  m_fileEvt.tid = ev->getTID();
  m_fileEvt.ret = ev->getSysCallResult();
  m_fileEvt.fileOID = file->file.oid;
  m_fileEvt.newFileOID.set_null();

  SF_DEBUG(m_logger, "The Current working Directory of the "
                         << ev->getName() << " event is " << ev->getCWD());
  m_writer->writeFileEvent(&m_fileEvt);
  return 0;
}

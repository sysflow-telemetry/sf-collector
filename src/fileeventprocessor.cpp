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

int FileEventProcessor::handleFileFlowEvent(sinsp_evt *ev, OpFlags flag) {
  int res = 1;
  if (flag == OP_MKDIR || flag == OP_RMDIR || flag == OP_UNLINK) {
    res = writeFileEvent(ev, flag);
  } else if (flag == OP_LINK || flag == OP_SYMLINK || flag == OP_RENAME) {
    res = writeLinkEvent(ev, flag);
  }

  return res;
}

int FileEventProcessor::writeLinkEvent(sinsp_evt *ev, OpFlags flag) {
  sinsp_threadinfo *ti = ev->get_thread_info();
  bool created = false;
  ProcessObj *proc = m_processCxt->getProcess(ev, SFObjectState::REUP, created);

  FileObj *file1 = nullptr;
  FileObj *file2 = nullptr;

  string path1;
  string path2;
  if (flag == OP_LINK || flag == OP_RENAME) {
    path1 = utils::getPath(ev, "oldpath");
    path2 = utils::getPath(ev, "newpath");
    if (IS_AT_SC(ev->get_type())) {
      int64_t olddirfd;
      int64_t newdirfd;
      if (flag == OP_RENAME) {
        olddirfd = utils::getFD(ev, "olddirfd");
        newdirfd = utils::getFD(ev, "newdirfd");
      } else {
        olddirfd = utils::getFD(ev, "olddir");
        newdirfd = utils::getFD(ev, "newdir");
      }
      path1 = utils::getAbsolutePath(ti, olddirfd, path1);
      path2 = utils::getAbsolutePath(ti, newdirfd, path2);

    } else {
      path1 = utils::getAbsolutePath(ti, path1);
      path2 = utils::getAbsolutePath(ti, path2);
    }
  } else if (flag == OP_SYMLINK) {
    path1 = utils::getPath(ev, "target");
    path2 = utils::getPath(ev, "linkpath");
    if (IS_AT_SC(ev->get_type())) {
      uint64_t linkdirfd = utils::getFD(ev, "linkdirfd");
      path2 = utils::getAbsolutePath(ti, linkdirfd, path2);
    } else {
      path1 = utils::getAbsolutePath(ti, path1);
      path2 = utils::getAbsolutePath(ti, path2);
    }
  }
  SF_DEBUG(m_logger, "Path parameters for ev: " << ev->get_name() << " are "
                                                << path1
                                                << " Path2: " << path2);

  file1 = m_fileCxt->getFile(ev, path1, SF_UNK, SFObjectState::REUP, created);
  file2 =
      m_fileCxt->getFile(ev, path2, SF_UNK, SFObjectState::CREATED, created);

  m_fileEvt.opFlags = flag;
  m_fileEvt.ts = ev->get_ts();
  m_fileEvt.procOID.hpid = proc->proc.oid.hpid;
  m_fileEvt.procOID.createTS = proc->proc.oid.createTS;
  m_fileEvt.tid = ti->m_tid;
  m_fileEvt.ret = utils::getSyscallResult(ev);
  m_fileEvt.fileOID = file1->file.oid;
  m_fileEvt.newFileOID.set_FOID(file2->file.oid);

  SF_DEBUG(m_logger, "The Current working Directory of the "
                         << ev->get_name() << " event is " << ti->get_cwd());
  m_writer->writeFileEvent(&m_fileEvt);
  return 0;
}

int FileEventProcessor::writeFileEvent(sinsp_evt *ev, OpFlags flag) {
  sinsp_threadinfo *ti = ev->get_thread_info();
  bool created = false;
  ProcessObj *proc = m_processCxt->getProcess(ev, SFObjectState::REUP, created);
  sinsp_fdinfo_t *fdinfo = ev->get_fd_info();
  FileObj *file = nullptr;
  if (fdinfo != nullptr) {
    file = m_fileCxt->getFile(ev, fdinfo, SFObjectState::CREATED, created);
  } else {
    string fileName = (IS_UNLINKAT(ev->get_type()))
                          ? utils::getPath(ev, "name")
                          : utils::getPath(ev, "path");
    if (IS_AT_SC(ev->get_type())) {
      sinsp_evt_param *pinfo;
      pinfo = ev->get_param(1);
      assert(pinfo->m_len == sizeof(int64_t));
      int64_t dirfd = *reinterpret_cast<int64_t *>(pinfo->m_val);
      fileName = utils::getAbsolutePath(ti, dirfd, fileName);
    } else {
      fileName = utils::getAbsolutePath(ti, fileName);
    }
    FileType fileType =
        (flag == OP_MKDIR || flag == OP_RMDIR) ? SF_DIR : SF_UNK;
    file = m_fileCxt->getFile(ev, fileName, fileType, SFObjectState::CREATED,
                              created);
  }
  m_fileEvt.opFlags = flag;
  m_fileEvt.ts = ev->get_ts();
  m_fileEvt.procOID.hpid = proc->proc.oid.hpid;
  m_fileEvt.procOID.createTS = proc->proc.oid.createTS;
  m_fileEvt.tid = ti->m_tid;
  m_fileEvt.ret = utils::getSyscallResult(ev);
  m_fileEvt.fileOID = file->file.oid;
  m_fileEvt.newFileOID.set_null();

  SF_DEBUG(m_logger, "The Current working Directory of the "
                         << ev->get_name() << " event is " << ti->get_cwd());
  m_writer->writeFileEvent(&m_fileEvt);
  return 0;
}

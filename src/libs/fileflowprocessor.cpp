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

#include "fileflowprocessor.h"
#include "utils.h"
#include <boost/stacktrace.hpp>
#include <utility>

using fileflow::FileFlowProcessor;

CREATE_LOGGER(FileFlowProcessor, "sysflow.fileflow");

FileFlowProcessor::FileFlowProcessor(context::SysFlowContext *cxt,
                                     writer::SysFlowWriter *writer,
                                     process::ProcessContext *processCxt,
                                     DataFlowSet *dfSet,
                                     file::FileContext *fileCxt) {
  m_cxt = cxt;
  m_writer = writer;
  m_processCxt = processCxt;
  m_dfSet = dfSet;
  m_fileCxt = fileCxt;
}

FileFlowProcessor::~FileFlowProcessor() = default;

inline void FileFlowProcessor::populateFileFlow(
    FileFlowObj *ff, OpFlags flag, sinsp_evt *ev, ProcessObj *proc,
    FileObj *file, std::string flowkey, sinsp_fdinfo_t *fdinfo, int64_t fd) {
  sinsp_threadinfo *ti = ev->get_thread_info();
  ff->fileflow.opFlags = flag;
  ff->fileflow.ts = ev->get_ts();
  ff->fileflow.openFlags = 0;
  if (flag == OP_OPEN) {
    ff->fileflow.openFlags = fdinfo->m_openflags;
  }
  ff->fileflow.endTs = 0;
  ff->fileflow.procOID.hpid = proc->proc.oid.hpid;
  ff->fileflow.procOID.createTS = proc->proc.oid.createTS;
  ff->fileflow.tid = ti->m_tid;
  ff->fileflow.tCapEffective = sinsp_utils::caps_to_string(ti->m_cap_effective);
  ff->fileflow.tCapInheritable =
      sinsp_utils::caps_to_string(ti->m_cap_inheritable);
  ff->fileflow.tCapPermitted = sinsp_utils::caps_to_string(ti->m_cap_permitted);
  ff->fileflow.fd = fd;
  ff->fileflow.fileOID = file->file.oid;
  if (!m_cxt->isConsumerMode()) {
    ff->filekey = file->key;
    ff->flowkey = std::move(flowkey);
  }
  ff->fileflow.numRRecvOps = 0;
  ff->fileflow.numWSendOps = 0;
  ff->fileflow.numRRecvBytes = 0;
  ff->fileflow.numWSendBytes = 0;
}

void FileFlowProcessor::removeAndWriteRelatedFlows(ProcessObj *proc,
                                                   FileFlowObj *ffo,
                                                   uint64_t endTs) {
  std::vector<FileFlowObj *> ffobjs;
  for (FileFlowTable::iterator ffi = proc->fileflows.begin();
       ffi != proc->fileflows.end(); ffi++) {
    if (ffi->second->fileflow.tid != ffo->fileflow.tid &&
        ffi->second->fileflow.fd == ffo->fileflow.fd &&
        ffi->second->filekey.compare(ffo->filekey) == 0) {
      if (ffi->second->fileflow.opFlags & OP_OPEN) {
        ffobjs.insert(ffobjs.begin(), ffi->second);
      } else {
        ffobjs.push_back(ffi->second);
      }
      SF_DEBUG(m_logger, "Removing related file flow on thread: "
                             << ffi->second->fileflow.tid);
      proc->fileflows.erase(ffi);
    }
  }
  for (auto it = ffobjs.begin(); it != ffobjs.end(); it++) {
    (*it)->fileflow.endTs = endTs;
    (*it)->fileflow.opFlags |= OP_TRUNCATE;
    // m_writer->writeFileFlow(&((*it)->fileflow));
    FileObj *file = m_fileCxt->getFile((*it)->filekey);
    SHOULD_WRITE((*it), &(proc->proc),
                 ((file != nullptr) ? &(file->file) : nullptr))
    removeFileFlowFromSet(&(*it), true);
  }
}

inline void FileFlowProcessor::updateFileFlow(FileFlowObj *ff, OpFlags flag,
                                              sinsp_evt *ev,
                                              sinsp_fdinfo_t *fdinfo) {
  ff->fileflow.opFlags |= flag;
  ff->lastUpdate = utils::getCurrentTime(m_cxt);
  if (flag == OP_OPEN) {
    ff->fileflow.openFlags = fdinfo->m_openflags;
  } else if (flag == OP_WRITE_SEND) {
    ff->fileflow.numWSendOps++;
    int res = utils::getSyscallResult(ev);
    if (res > 0) {
      ff->fileflow.numWSendBytes += res;
    }
  } else if (flag == OP_READ_RECV) {
    ff->fileflow.numRRecvOps++;
    int res = utils::getSyscallResult(ev);
    if (res > 0) {
      ff->fileflow.numRRecvBytes += res;
    }
  }
}

inline void FileFlowProcessor::processNewFlow(sinsp_evt *ev, ProcessObj *proc,
                                              FileObj *file, OpFlags flag,
                                              const std::string &flowkey,
                                              sinsp_fdinfo_t *fdinfo,
                                              int64_t fd) {
  auto *ff = new FileFlowObj();
  ff->exportTime = utils::getCurrentTime(m_cxt);
  ff->lastUpdate = utils::getCurrentTime(m_cxt);
  populateFileFlow(ff, flag, ev, proc, file, flowkey, fdinfo, fd);
  updateFileFlow(ff, flag, ev, fdinfo);
  if (flag != OP_CLOSE) {
    proc->fileflows[ff->flowkey] = ff;
    file->refs++;
    m_dfSet->insert(ff);
  } else {
    removeAndWriteRelatedFlows(proc, ff, ev->get_ts());
    ff->fileflow.endTs = ev->get_ts();
    // m_writer->writeFileFlow(&(ff->fileflow));
    SHOULD_WRITE(ff, &(proc->proc), &(file->file))
    delete ff;
  }
}

inline int FileFlowProcessor::createConsumerRecord(sinsp_evt *ev,
                                                   ProcessObj *proc,
                                                   FileObj *file, OpFlags flag,
                                                   sinsp_fdinfo_t *fdinfo,
                                                   int64_t fd) {
  if (flag == OP_CLOSE || flag == OP_SHUTDOWN) {
    return 1;
  }

  if (flag != OP_OPEN) {
    SF_WARN(m_logger, "Received a file flow flag other than open or close in "
                      "consumer mode. Flag: "
                          << flag)
    return 1;
  }
  FileFlowObj ffobj;
  static std::string fk;
  populateFileFlow(&ffobj, flag, ev, proc, file, fk, fdinfo, fd);
  ffobj.fileflow.endTs = ev->get_ts();
  SHOULD_WRITE((&ffobj), &(proc->proc), &(file->file))
  return 1;
}

inline void FileFlowProcessor::removeAndWriteFileFlow(ProcessObj *proc,
                                                      FileObj *file,
                                                      FileFlowObj **ff,
                                                      std::string flowkey) {
  // m_writer->writeFileFlow(&((*ff)->fileflow));
  SHOULD_WRITE((*ff), &(proc->proc), &(file->file))
  removeFileFlowFromSet(ff, false);
  removeFileFlow(proc, file, ff, std::move(flowkey));
}

inline void FileFlowProcessor::processExistingFlow(
    sinsp_evt *ev, ProcessObj *proc, FileObj *file, OpFlags flag,
    std::string flowkey, FileFlowObj *ff, sinsp_fdinfo_t *fdinfo) {
  updateFileFlow(ff, flag, ev, fdinfo);
  if (flag == OP_CLOSE) {
    removeAndWriteRelatedFlows(proc, ff, ev->get_ts());
    ff->fileflow.endTs = ev->get_ts();
    removeAndWriteFileFlow(proc, file, &ff, std::move(flowkey));
  }
}

int FileFlowProcessor::handleFileFlowEvent(sinsp_evt *ev, OpFlags flag) {
  sinsp_fdinfo_t *fdinfo = ev->get_fd_info();
  int64_t fd = ev->get_fd_num();

  if (fdinfo == nullptr) {
    SF_DEBUG(m_logger,
             "Event: " << ev->get_name()
                       << " doesn't have a fdinfo associated with it!");
    if (flag == OP_MMAP) {
      if (fd != sinsp_evt::INVALID_FD_NUM) {
        sinsp_threadinfo *ti = ev->get_thread_info();
        fdinfo = ti->get_fd(fd);

      } else {
        fd = utils::getFD(ev);
        if (!utils::isMapAnonymous(ev) && fd != -1) {
          sinsp_threadinfo *ti = ev->get_thread_info();
          fdinfo = ti->get_fd(fd);
        }
      }
    }

    if (fdinfo == nullptr) {
      return 1;
    }
  }

  if ((fdinfo->is_ipv4_socket() || fdinfo->is_ipv6_socket())) {
    SF_DEBUG(m_logger,
             "handleFileFlowEvent cannot handle ip sockets, ignoring...");
    return 1;
  }
  char restype = fdinfo->get_typechar();

  if (m_cxt->isFileOnly() && !fdinfo->is_file()) {
    return 1;
  }

  switch (restype) {
  case SF_FILE:
  case SF_DIR:
  case SF_UNIX:
  case SF_PIPE:
    break;
  default:
    return 1;
  }

  bool created = false;
  // calling get process is important because it ensures that the process object
  // has been written to the sysflow file. This is important for long running
  // NetworkFlows that may span across files.
  ProcessObj *proc = m_processCxt->getProcess(ev, SFObjectState::REUP, created);
  FileObj *file = m_fileCxt->getFile(ev, fdinfo, SFObjectState::REUP, created);
  sinsp_threadinfo *ti = ev->get_thread_info();
  if (m_cxt->isConsumerMode()) {
    return createConsumerRecord(ev, proc, file, flag, fdinfo, fd);
  }
  FileFlowObj *ff = nullptr;
  std::string flowkey;
  flowkey.reserve(ti->m_container_id.length() + fdinfo->m_name.length() + 32);
  flowkey += fdinfo->m_name;
  flowkey += ti->m_container_id;
  flowkey.append(utils::itoa(ti->m_tid, 10));
  flowkey.append(utils::itoa(fd, 10));

  FileFlowTable::iterator ffi = proc->fileflows.find(flowkey);
  if (ffi != proc->fileflows.end()) {
    ff = ffi->second;
  }
  SF_DEBUG(m_logger, proc->proc.exe << " Name: " << fdinfo->m_name
                                    << " type: " << fdinfo->get_typechar()
                                    << " " << file->file.path << " "
                                    << ev->get_name());

  if (ff == nullptr) {
    processNewFlow(ev, proc, file, flag, flowkey, fdinfo, fd);
  } else {
    processExistingFlow(ev, proc, file, flag, flowkey, ff, fdinfo);
  }

  return 0;
}

void FileFlowProcessor::removeFileFlow(ProcessObj *proc, FileObj *file,
                                       FileFlowObj **ff,
                                       const std::string &flowkey) {
  proc->fileflows.erase(flowkey);
  delete *ff;
  ff = nullptr;
  if (file != nullptr) {
    file->refs--;
  }
}

int FileFlowProcessor::removeAndWriteFFFromProc(ProcessObj *proc, int64_t tid) {
  SF_DEBUG(m_logger, "CALLING removeAndWriteFFFromProc");
  int deleted = 0;

  for (FileFlowTable::iterator ffi = proc->fileflows.begin();
       ffi != proc->fileflows.end(); ffi++) {
    if (tid == -1 || tid == ffi->second->fileflow.tid) {
      FileObj *file = m_fileCxt->getFile(ffi->second->filekey);
      ffi->second->fileflow.endTs = utils::getSinspTime(m_cxt);
      if (tid != -1) {
        removeAndWriteRelatedFlows(proc, ffi->second,
                                   ffi->second->fileflow.endTs);
      }
      ffi->second->fileflow.opFlags |= OP_TRUNCATE;
      SF_DEBUG(m_logger, "Writing FILEFLOW!");
      SHOULD_WRITE(ffi->second, &(proc->proc), &(file->file))
      // m_writer->writeFileFlow(&(ffi->second->fileflow));
      FileFlowObj *ffo = ffi->second;
      proc->fileflows.erase(ffi);
      SF_DEBUG(m_logger, "Set size: " << m_dfSet->size());
      deleted += removeFileFlowFromSet(&ffo, true);
      SF_DEBUG(m_logger, "After Set size: " << m_dfSet->size());
      if (file == nullptr) {
        SF_ERROR(m_logger, "File object doesn't exist for fileflow: "
                               << ffi->second->filekey
                               << ". This shouldn't happen.");
      } else {
        file->refs--;
      }
    }
  }

  if (tid == -1) {
    proc->fileflows.clear();
  }

  return deleted;
}

int FileFlowProcessor::removeFileFlowFromSet(FileFlowObj **ffo,
                                             bool deleteFileFlow) {
  bool found = false;
  int removed = 0;

  for (auto iter = m_dfSet->find(*ffo); iter != m_dfSet->end(); iter++) {
    if (!((*iter)->isNetworkFlow)) {
      auto *foundObj = static_cast<FileFlowObj *>(*iter);
      if (*foundObj == **ffo) {
        SF_DEBUG(m_logger, "Removing fileflow element from multiset");
        m_dfSet->erase(iter);
        if (deleteFileFlow) {
          delete *ffo;
          ffo = nullptr;
        }
        removed++;
        found = true;
        break;
      }
    }
  }

  if (!found) {
    SF_ERROR(m_logger,
             "Cannot find FileFlow Object "
                 << (*ffo)->filekey << " " << (*ffo)->flowkey << " "
                 << (*ffo)->fileflow.opFlags << " " << (*ffo)->fileflow.endTs
                 << " " << boost::stacktrace::stacktrace()
                 << " in data flow set. Deleting. This should not happen.");
    ProcessObj *proc = m_processCxt->getProcess(&((*ffo)->fileflow.procOID));
    if (proc == nullptr) {
      SF_ERROR(m_logger, "Could not find proc "
                             << (*ffo)->fileflow.procOID.hpid << " "
                             << (*ffo)->fileflow.procOID.createTS
                             << " This shouldn't happen!");
    } else {
      FileObj *file = m_fileCxt->getFile((*ffo)->filekey);
      if (file == nullptr) {
        SF_ERROR(m_logger, "Unable to find file object of key "
                               << (*ffo)->filekey << ". Shouldn't happen!");
      } else {
        SF_ERROR(m_logger,
                 "Proc name: " << proc->proc.exe << " " << proc->proc.exeArgs
                               << " Pid: " << proc->proc.oid.hpid << " "
                               << " File Name: " << file->file.path << " "
                               << " File type: " << file->file.restype);
      }
    }

    if (deleteFileFlow) {
      SF_ERROR(m_logger, "Deleting File Flow...");
      delete *ffo;
      ffo = nullptr;
      SF_ERROR(m_logger, "Deleted File Flow...");
    }
  }

  return removed;
}

void FileFlowProcessor::removeFileFlow(DataFlowObj *dfo) {
  auto *ffo = static_cast<FileFlowObj *>(dfo);
  // do we want to write out a fileflow that hasn't had any action in an
  // interval? nfo->fileflow.endTs = utils::getSinspTime(m_cxt);
  // m_writer->writeNetFlow(&(nfo->fileflow));
  SF_DEBUG(m_logger, "Erasing flow");
  ProcessObj *proc = m_processCxt->getProcess(&(ffo->fileflow.procOID));
  if (proc == nullptr) {
    SF_ERROR(m_logger, "Could not find proc " << ffo->fileflow.procOID.hpid
                                              << " "
                                              << ffo->fileflow.procOID.createTS
                                              << " This shouldn't happen!");
  } else {
    FileObj *file = m_fileCxt->getFile(ffo->filekey);
    if (file == nullptr) {
      SF_ERROR(m_logger, "Unable to find file object of key "
                             << ffo->filekey << ". Shouldn't happen!");
    }
    removeFileFlow(proc, file, &ffo, ffo->flowkey);
  }
}

void FileFlowProcessor::exportFileFlow(DataFlowObj *dfo, time_t /*now*/) {
  auto *ffo = static_cast<FileFlowObj *>(dfo);
  ffo->fileflow.endTs = utils::getSinspTime(m_cxt);
  ProcessObj *proc = m_processCxt->exportProcess(&(ffo->fileflow.procOID));
  FileObj *file = m_fileCxt->exportFile(ffo->filekey);
  SHOULD_WRITE(ffo, ((proc != nullptr) ? &(proc->proc) : nullptr),
               ((file != nullptr) ? &(file->file) : nullptr))
  // m_writer->writeFileFlow(&(ffo->fileflow));
  SF_DEBUG(m_logger, "Reupping flow");
  ffo->fileflow.ts = utils::getSinspTime(m_cxt);
  ffo->fileflow.endTs = 0;
  ffo->fileflow.opFlags = 0;
  ffo->fileflow.numRRecvOps = 0;
  ffo->fileflow.numWSendOps = 0;
  ffo->fileflow.numRRecvBytes = 0;
  ffo->fileflow.numWSendBytes = 0;
}

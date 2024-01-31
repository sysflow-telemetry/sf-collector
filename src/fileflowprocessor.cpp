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
    FileFlowObj *ff, OpFlags flag, api::SysFlowEvent *ev, ProcessObj *proc,
    FileObj *file, string flowkey, api::SysFlowFileDescInfo *fdinfo,
    int64_t fd) {
  ff->fileflow.opFlags = flag;
  ff->fileflow.ts = ev->getTS();
  ff->fileflow.openFlags = 0;
  if (flag == OP_OPEN) {
    ff->fileflow.openFlags = fdinfo->getOpenFlag();
  }
  ff->fileflow.endTs = 0;
  ff->fileflow.procOID.hpid = proc->proc.oid.hpid;
  ff->fileflow.procOID.createTS = proc->proc.oid.createTS;
  ff->fileflow.tid = ev->getTID();
  ff->fileflow.fd = fd;
  ff->fileflow.fileOID = file->file.oid;
  ff->filekey = file->key;
  ff->flowkey = std::move(flowkey);
  ff->fileflow.numRRecvOps = 0;
  ff->fileflow.numWSendOps = 0;
  ff->fileflow.numRRecvBytes = 0;
  ff->fileflow.numWSendBytes = 0;
}

void FileFlowProcessor::removeAndWriteRelatedFlows(ProcessObj *proc,
                                                   FileFlowObj *ffo,
                                                   uint64_t endTs) {
  vector<FileFlowObj *> ffobjs;
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
    m_writer->writeFileFlow(&((*it)->fileflow));
    removeFileFlowFromSet(&(*it), true);
  }
}

inline void
FileFlowProcessor::updateFileFlow(FileFlowObj *ff, OpFlags flag,
                                  api::SysFlowEvent *ev,
                                  api::SysFlowFileDescInfo *fdinfo) {
  ff->fileflow.opFlags |= flag;
  ff->lastUpdate = utils::getCurrentTime(m_cxt);
  if (flag == OP_OPEN) {
    ff->fileflow.openFlags = fdinfo->getOpenFlag();
  } else if (flag == OP_WRITE_SEND) {
    ff->fileflow.numWSendOps++;
    int res = ev->getSysCallResult();
    if (res > 0) {
      ff->fileflow.numWSendBytes += res;
    }
  } else if (flag == OP_READ_RECV) {
    ff->fileflow.numRRecvOps++;
    int res = ev->getSysCallResult();
    if (res > 0) {
      ff->fileflow.numRRecvBytes += res;
    }
  }
}

inline void FileFlowProcessor::processNewFlow(
    api::SysFlowEvent *ev, ProcessObj *proc, FileObj *file, OpFlags flag,
    const string &flowkey, api::SysFlowFileDescInfo *fdinfo, int64_t fd) {
  auto *ff = new FileFlowObj();
  ff->exportTime = utils::getExportTime(m_cxt);
  ff->lastUpdate = utils::getCurrentTime(m_cxt);
  populateFileFlow(ff, flag, ev, proc, file, flowkey, fdinfo, fd);
  updateFileFlow(ff, flag, ev, fdinfo);
  if (flag != OP_CLOSE && flag != OP_LOAD_LIBRARY) {
    proc->fileflows[ff->flowkey] = ff;
    file->refs++;
    m_dfSet->insert(ff);
  } else {
    removeAndWriteRelatedFlows(proc, ff, ev->getTS());
    ff->fileflow.endTs = ev->getTS();
    m_writer->writeFileFlow(&(ff->fileflow));
    delete ff;
  }
}

inline void FileFlowProcessor::removeAndWriteFileFlow(ProcessObj *proc,
                                                      FileObj *file,
                                                      FileFlowObj **ff,
                                                      string flowkey) {
  m_writer->writeFileFlow(&((*ff)->fileflow));
  removeFileFlowFromSet(ff, false);
  removeFileFlow(proc, file, ff, std::move(flowkey));
}

inline void FileFlowProcessor::processExistingFlow(
    api::SysFlowEvent *ev, ProcessObj *proc, FileObj *file, OpFlags flag,
    string flowkey, FileFlowObj *ff, api::SysFlowFileDescInfo *fdinfo) {
  updateFileFlow(ff, flag, ev, fdinfo);
  if (flag == OP_CLOSE) {
    removeAndWriteRelatedFlows(proc, ff, ev->getTS());
    ff->fileflow.endTs = ev->getTS();
    removeAndWriteFileFlow(proc, file, &ff, std::move(flowkey));
  }
}

int FileFlowProcessor::handleFileFlowEvent(api::SysFlowEvent *ev) {
  api::SysFlowFileDescInfo *fdinfo = ev->getFileDescInfo();
  OpFlags flag = ev->opFlag;
  if (fdinfo == nullptr) {
    SF_DEBUG(m_logger,
             "Event: " << ev->getName()
                       << " doesn't have a fdinfo associated with it!");
    return 1;
  }

  if (fdinfo->isIPSocket()) {
    SF_WARN(m_logger,
            "handleFileFlowEvent cannot handle ip sockets, ignoring...");
    return 1;
  }
  int64_t fd = ev->getFD();
  char restype = fdinfo->getFileType();

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
  FileFlowObj *ff = nullptr;
  string filekey = ev->getContainerID() + fdinfo->getName();
  string flowkey = filekey + std::to_string(ev->getTID()) + std::to_string(fd);

  FileObj *file = m_fileCxt->getFile(ev, fdinfo, SFObjectState::REUP, created);
  FileFlowTable::iterator ffi = proc->fileflows.find(flowkey);
  if (ffi != proc->fileflows.end()) {
    ff = ffi->second;
  }
  SF_DEBUG(m_logger, proc->proc.exe << " Name: " << fdinfo->getName()
                                    << " type: " << fdinfo->getFileType() << " "
                                    << file->file.path << " " << ev->getName());

  if (ff == nullptr) {
    processNewFlow(ev, proc, file, flag, flowkey, fdinfo, fd);
  } else {
    processExistingFlow(ev, proc, file, flag, flowkey, ff, fdinfo);
  }
  return 0;
}

void FileFlowProcessor::removeFileFlow(ProcessObj *proc, FileObj *file,
                                       FileFlowObj **ff,
                                       const string &flowkey) {
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
      ffi->second->fileflow.endTs = utils::getSystemTime(m_cxt);
      if (tid != -1) {
        removeAndWriteRelatedFlows(proc, ffi->second,
                                   ffi->second->fileflow.endTs);
      }
      ffi->second->fileflow.opFlags |= OP_TRUNCATE;
      SF_DEBUG(m_logger, "Writing FILEFLOW!");
      m_writer->writeFileFlow(&(ffi->second->fileflow));
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
        iter = m_dfSet->erase(iter);
        if (deleteFileFlow) {
          delete *ffo;
          *ffo = nullptr;
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
      *ffo = nullptr;
      SF_ERROR(m_logger, "Deleted File Flow...");
    }
  }
  return removed;
}

void FileFlowProcessor::removeFileFlow(DataFlowObj *dfo) {
  auto *ffo = static_cast<FileFlowObj *>(dfo);
  // do we want to write out a fileflow that hasn't had any action in an
  // interval? nfo->fileflow.endTs = utils::getSystemTime(m_cxt);
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
  ffo->fileflow.endTs = utils::getSystemTime(m_cxt);
  m_processCxt->exportProcess(&(ffo->fileflow.procOID));
  m_fileCxt->exportFile(ffo->filekey);
  m_writer->writeFileFlow(&(ffo->fileflow));
  SF_DEBUG(m_logger, "Reupping flow");
  ffo->fileflow.ts = utils::getSystemTime(m_cxt);
  ffo->fileflow.endTs = 0;
  ffo->fileflow.opFlags = 0;
  ffo->fileflow.numRRecvOps = 0;
  ffo->fileflow.numWSendOps = 0;
  ffo->fileflow.numRRecvBytes = 0;
  ffo->fileflow.numWSendBytes = 0;
}

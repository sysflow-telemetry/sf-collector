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

#include "sysflowprocessor.h"
#include "sfcallbackwriter.h"

using sysflowprocessor::SysFlowProcessor;

CREATE_LOGGER(SysFlowProcessor, "sysflow.sysflowprocessor");

SysFlowProcessor::SysFlowProcessor(context::SysFlowContext *cxt, writer::SysFlowWriter *writer)
    : m_exit(false) {
  m_cxt = cxt;
  time_t start = 0;
  if (m_cxt->getFileDuration() > 0) {
    start = utils::getCurrentTime(m_cxt);
  }
  if (m_cxt->isStatsEnabled()) {
    m_statsTime = utils::getCurrentTime(m_cxt);
  } else {
    m_statsTime = 0;
  }
  if (writer == nullptr) {
    if (m_cxt->isDomainSocket() && m_cxt->isOutputFile()) {
      m_writer = new writer::SFMultiWriter(cxt, start);
    } else if (m_cxt->isOutputFile()) {
      m_writer = new writer::SFFileWriter(cxt, start);
    } else if (m_cxt->isDomainSocket()) {
      m_writer = new writer::SFSocketWriter(cxt, start);
    } else if (m_cxt->hasCallback()) {
      m_writer = new writer::SFCallbackWriter(cxt, start, m_cxt->getCallback(), this);
    } else {
      SF_ERROR(m_logger, "Neither file output (-w) or (-u) were set. At least "
                       "one must be specified.")
      ::exit(EXIT_FAILURE);
    }
  } else {
    m_writer = writer;
  }
  m_k8sCxt = nullptr;
  if(m_cxt->isK8sEnabled()) {
    m_k8sCxt = new sfk8s::K8sContext(m_cxt, m_writer);
    m_k8sPrcr = new k8sevent::K8sEventProcessor(m_writer, m_k8sCxt);
  }
  m_containerCxt = new container::ContainerContext(m_cxt, m_writer, m_k8sCxt);
  m_fileCxt = new file::FileContext(m_containerCxt, m_writer);
  m_processCxt =
      new process::ProcessContext(m_cxt, m_containerCxt, m_fileCxt, m_writer);
  m_dfPrcr =
      new dataflow::DataFlowProcessor(m_cxt, m_writer, m_processCxt, m_fileCxt);
  m_ctrlPrcr = new controlflow::ControlFlowProcessor(m_cxt, m_writer,
                                                     m_processCxt, m_dfPrcr);
}

SysFlowProcessor::~SysFlowProcessor() {
  delete m_dfPrcr;
  delete m_ctrlPrcr;
  if (m_k8sCxt != nullptr) {
    delete m_k8sCxt;
  }
  delete m_containerCxt;
  delete m_processCxt;
  delete m_fileCxt;
  delete m_writer;
  delete m_cxt;
}

void SysFlowProcessor::clearTables() {
  m_processCxt->clearProcesses();
  m_containerCxt->clearContainers();
  if(m_cxt->isK8sEnabled()) {
    m_k8sCxt->clearPods();
  }
  m_fileCxt->clearFiles();
}

bool SysFlowProcessor::checkAndRotateFile() {
  bool fileRotated = false;
  time_t curTime = utils::getCurrentTime(m_cxt);
  if (m_writer->isExpired(curTime) || m_writer->needsReset()) {
    SF_INFO(m_logger,
            "Container Table: "
                << m_containerCxt->getSize()
		<< " K8s Enabled: " << m_cxt->isK8sEnabled()
		<< " Pods Table: " << ( m_cxt->isK8sEnabled() ? m_k8sCxt->getSize() : 0 )
                << " Process Table: " << m_processCxt->getSize()
                << " NetworkFlow Table: " << m_dfPrcr->getNFSize()
                << " FileFlow Table: " << m_dfPrcr->getFFSize()
                << " ProcFlow Table: " << m_ctrlPrcr->getSize()
                << " Num Records Written: " << m_writer->getNumRecs());
    m_writer->reset(curTime);
    clearTables();
    fileRotated = true;
  }
  if (m_statsTime > 0) {
    double duration = difftime(curTime, m_statsTime);
    if (duration >= m_cxt->getStatsInterval()) {
      m_dfPrcr->printFlowStats();
      m_statsTime = curTime;
    }
  }
  return fileRotated;
}

int SysFlowProcessor::checkForExpiredRecords() {
  int numExpired = m_dfPrcr->checkForExpiredRecords();
  if (numExpired) {
    SF_DEBUG(m_logger, "Data Flow Records exported: " << numExpired);
  }
  int numProcExpired = m_ctrlPrcr->checkForExpiredRecords();
  if (numProcExpired) {
    SF_DEBUG(m_logger, "Data Flow Records exported: " << numProcExpired);
  }
  return numExpired + numProcExpired;
}

int SysFlowProcessor::run() {
  int32_t res = 0;
  sinsp_evt *ev = nullptr;
  try {
    m_writer->initialize();
    while (true) {
      res = m_cxt->getInspector()->next(&ev);
      if (res == SCAP_TIMEOUT) {
        if (m_exit) {
          break;
        }
        checkForExpiredRecords();
        m_processCxt->checkForDeletion();
        checkAndRotateFile();
        continue;
      } else if (res == SCAP_EOF) {
        break;
      } else if (res != SCAP_SUCCESS) {
        SF_ERROR(m_logger, "SCAP processor failed with res = "
                               << res << " and error: "
                               << m_cxt->getInspector()->getlasterr());
        throw sinsp_exception(m_cxt->getInspector()->getlasterr().c_str());
      }
      m_cxt->timeStamp = ev->get_ts();
      if (m_exit) {
        break;
      }
      checkForExpiredRecords();
      m_processCxt->checkForDeletion();
      checkAndRotateFile();
      if (m_cxt->isFilterContainers() && !utils::isInContainer(ev)) {
        continue;
      }
      if ( m_cxt->getInspector()->m_k8s_client != nullptr &&
		      m_cxt->getInspector()->m_k8s_client->get_capture_events().size() > 0) {
        SF_INFO(m_logger, "Events Count: " << m_cxt->getInspector()->m_k8s_client->get_capture_events().size());
      }
      switch (ev->get_type()) {
        SF_EXECVE_ENTER()
        SF_EXECVE_EXIT(ev)
        SF_CLONE_EXIT(ev)
        SF_PROCEXIT_E_X(ev)
        SF_OPEN_EXIT(ev)
        SF_ACCEPT_EXIT(ev)
        SF_CONNECT_EXIT(ev)
        SF_SEND_EXIT(ev)
        SF_RECV_EXIT(ev)
        SF_CLOSE_EXIT(ev)
        SF_SETNS_EXIT(ev)
        SF_MKDIR_EXIT(ev)
        SF_RMDIR_EXIT(ev)
        SF_LINK_EXIT(ev)
        SF_UNLINK_EXIT(ev)
        SF_SYMLINK_EXIT(ev)
        SF_RENAME_EXIT(ev)
        SF_SETUID_ENTER(ev)
        SF_SETUID_EXIT(ev)
        SF_SHUTDOWN_EXIT(ev)
        SF_MMAP_EXIT(ev)
	case PPME_K8S_E:
	{
          std::cout << "Received a k8s event!!!" << std::endl;
	  if(m_cxt->isK8sEnabled()) {
	    m_k8sPrcr->handleK8sEvent(ev);
	  }
	  break;
        }
      }	
    }
    SF_INFO(m_logger, "Exiting scap loop... shutting down");
    SF_INFO(m_logger,
            "Container Table: "
                << m_containerCxt->getSize()
		<< " K8s Enabled: " << m_cxt->isK8sEnabled()
		<< " Pods Table: " << ( m_cxt->isK8sEnabled() ? m_k8sCxt->getSize() : 0)
                << " Process Table: " << m_processCxt->getSize()
                << " NetworkFlow Table: " << m_dfPrcr->getNFSize()
                << " FileFlow Table: " << m_dfPrcr->getFFSize()
                << " ProcFlow Table: " << m_ctrlPrcr->getSize()
                << " Num Records Written: " << m_writer->getNumRecs());
  } catch (sinsp_exception &e) {
    SF_ERROR(m_logger, "Sysdig exception " << e.what());
    return 1;
  } catch (avro::Exception &ex) {
    SF_ERROR(m_logger, "Avro Exception! Error: " << ex.what());
    return 1;
  }
  return 0;
}


sysflow::Container* SysFlowProcessor::getContainer(const string& id) {
  ContainerObj* cont = m_containerCxt->getContainer(id);
  if (cont != nullptr) {
    return &(cont->cont);
  }
  return nullptr;
}


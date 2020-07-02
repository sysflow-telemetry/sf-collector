#include "sysdig/sdinspector.h"

using namespace sysdig;

SysdigInspector::SysdigInspector(context::SysFlowContext *cxt)
    : api::SysFlowInspector(cxt) {
  m_inspector = new sinsp();
}

int SysdigInspector::init() {
  m_inspector->set_hostname_and_port_resolution_mode(false);
  if (!m_cxt->getFilter().empty()) {
    m_inspector->set_filter(m_cxt->getFilter());
  }
  if (!m_cxt->getCriPath().empty()) {
    m_inspector->set_cri_socket_path(m_cxt->getCriPath());
  }
  if (m_cxt->getCriTO() > 0) {
    m_inspector->set_cri_timeout(m_cxt->getCriTO());
  }
  const char *envP = std::getenv(SYSDIG_LOG);
  if (envP != nullptr && strcmp(envP, "1") == 0) {
    m_inspector->set_log_stderr();
    m_inspector->set_min_log_severity(sinsp_logger::severity::SEV_DEBUG);
  }
  initExporterID();
  try {
    m_inspector->open(m_cxt->getInputFile());
  } catch (sinsp_exception &ex) {
    throw std::system_error(EIO, std::generic_category(), ex.what());
  }
  return 1;
}

int SysdigInspector::cleanup() {
  if (m_inspector != nullptr) {
    m_inspector->close();
  }
  return 1;
}

SysdigInspector::~SysdigInspector() {
  if (m_inspector != nullptr) {
    m_inspector->close();
    delete m_inspector;
  }
}
void SysdigInspector::initExporterID() {
  if (m_cxt->getExporterID().empty()) {
    const scap_machine_info *mi = m_inspector->get_machine_info();
    if (mi != nullptr) {
      m_cxt->setExporterID(mi->hostname);
    } else {
      char host[257];
      memset(host, 0, 257);
      if (gethostname(host, 256)) {
        SF_ERROR(m_logger,
                 "Error calling gethostname for sysflow header. Error Code: "
                     << std::strerror(errno));
        exit(1);
      }
      m_cxt->setExporterID(host);
    }
  }
}

int SysdigInspector::next(api::SysFlowEvent **e) {
  *e = &evt;
  int res = m_inspector->next(&(evt.ev));
  evt.init(m_inspector);
  return res;
}

std::string SysdigInspector::getLastError() {
  return m_inspector->getlasterr();
}

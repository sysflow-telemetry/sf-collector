#include "sysflowcontext.h"

SysFlowContext::SysFlowContext(bool fCont, int fDur, bool prefix, string oFile, string sFile, string schFile, string expID ) : m_filterCont(fCont),  m_fileDuration(fDur),  m_hasPrefix(prefix),
       m_outputFile(oFile), m_scapFile(sFile), m_schemaFile(schFile), m_exporterID(expID), m_nfExportInterval(30), m_nfExpireInterval(30) {
   m_inspector = new sinsp();
   m_inspector->set_hostname_and_port_resolution_mode(false);
   m_inspector->open(m_scapFile);
   m_offline = !sFile.empty();
}

SysFlowContext::~SysFlowContext() {
   if(m_inspector != NULL) {
      m_inspector->close();
      delete m_inspector;
   }
}


#include "sysflowcontext.h"

//LoggerPtr SysFlowContext::m_logger(Logger::getLogger("sysflow.sysflowcontext"));

SysFlowContext::SysFlowContext(bool fCont, int fDur, string oFile, string sFile, string schFile, string expID, string filter) : m_filterCont(fCont),  m_fileDuration(fDur),
       m_outputFile(oFile), m_scapFile(sFile), m_schemaFile(schFile), m_exporterID(expID), m_nfExportInterval(30), m_nfExpireInterval(30), m_filter(filter) {
   m_inspector = new sinsp();
   m_inspector->set_hostname_and_port_resolution_mode(false);
   if(!m_filter.empty()) {
       m_inspector->set_filter(m_filter);
   }
   m_inspector->open(m_scapFile);
   m_offline = !sFile.empty();
   m_hasPrefix = (oFile.back() != '/');
}

SysFlowContext::~SysFlowContext() {
   if(m_inspector != NULL) {
      m_inspector->close();
      delete m_inspector;
   }
}
string SysFlowContext::getExporterID() {
    if(m_exporterID.empty()) {
         const scap_machine_info* mi = m_inspector->get_machine_info();
         if(mi != NULL) {
             m_exporterID = mi->hostname;
   	 } else {	
             char host[257];
             memset(host, 0, 257);
             if(gethostname(host, 256) ) {
                  SF_ERROR(m_logger, "Error calling gethostname for sysflow header. Error Code: " <<  std::strerror(errno));
                   exit(1); 
             }
             m_exporterID = host;
         }
    }
    return m_exporterID;
}


#include "sysflowprocessor.h"
using namespace sysflowprocessor;
CREATE_LOGGER(SysFlowProcessor, "sysflow.sysflowprocessor");

//SysFlowProcessor::SysFlowProcessor(SysFlowContext* cxt) : m_exit(false), m_cxt(NULL), m_writer(NULL), m_containerCxt(NULL), m_fileCxt(NULL), m_processCxt(NULL), m_procEvtPrcr(NULL), m_dfPrcr(NULL) {
SysFlowProcessor::SysFlowProcessor(SysFlowContext* cxt) : m_exit(false) {
   m_cxt = cxt;
   time_t start = 0;
   if(m_cxt->getFileDuration() > 0) {
       start = time(NULL);
   }
   m_writer = new SysFlowWriter(cxt, start);
   m_containerCxt = new container::ContainerContext(m_cxt, m_writer);
   m_fileCxt = new file::FileContext(m_containerCxt, m_writer);
   m_processCxt = new process::ProcessContext(m_cxt, m_containerCxt, m_fileCxt, m_writer);
   m_dfPrcr = new dataflow::DataFlowProcessor(m_cxt, m_writer, m_processCxt, m_fileCxt);
   m_procEvtPrcr = new processevent::ProcessEventProcessor(m_writer, m_processCxt, m_dfPrcr);
}

SysFlowProcessor::~SysFlowProcessor() {
   delete m_dfPrcr;
   delete m_procEvtPrcr;
   delete m_containerCxt;
   delete m_processCxt;
   delete m_fileCxt;
   delete m_writer;
   delete m_cxt;
}

void SysFlowProcessor::clearTables() {
   m_processCxt->clearProcesses();
   m_containerCxt->clearContainers();
   m_fileCxt->clearFiles();
}

bool SysFlowProcessor::checkAndRotateFile()  {
     bool fileRotated = false;
     time_t curTime = time(NULL);
     if(m_writer->isFileExpired(curTime)) {
         SF_INFO(m_logger, "Container Table: " << m_containerCxt->getSize() <<  " Process Table: " << m_processCxt->getSize() << " NetworkFlow Table: " << m_dfPrcr->getSize() << " Num Records Written: " << m_writer->getNumRecs());
         m_writer->resetFileWriter(curTime);
         clearTables();
         fileRotated = true;
     }
     return fileRotated;
}

int SysFlowProcessor::run() {
	int32_t res = 0;
	sinsp_evt* ev = NULL;
	try
	{
                m_writer->initialize();
		//inspector->set_buffer_format(sinsp_evt::PF_NORMAL);
		while(true) 
		{
			res = m_cxt->getInspector()->next(&ev);
			//cout << "Retrieved a scap event... RES: " << res << endl;
			if(res == SCAP_TIMEOUT)
			{
                                if(m_exit) {
                                    break;
                                }
                                int numExpired = m_dfPrcr->checkForExpiredRecords();
                                if(numExpired) {
                                    SF_INFO(m_logger, "Data Flow Records exported: " << numExpired );
                                }
				checkAndRotateFile();
				continue;
			}
			else if(res == SCAP_EOF)
			{
				break;
			}
			else if(res != SCAP_SUCCESS)
			{
				SF_ERROR(m_logger, "SCAP processor failed with res = " << res << " and error: " << m_cxt->getInspector()->getlasterr() );
				throw sinsp_exception(m_cxt->getInspector()->getlasterr().c_str());
			}
                        m_cxt->timeStamp = ev->get_ts();
         	        if(m_exit) {
                           break;
                        }
                        int numExpired = m_dfPrcr->checkForExpiredRecords();
                        if(numExpired) {
                            SF_INFO(m_logger, "Data Flow Records exported: " << numExpired );
                        }
                        checkAndRotateFile();
                        if(m_cxt->isFilterContainers() && !utils::isInContainer(ev)) {
                              continue;
                        }
			switch(ev->get_type()) {
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
                       } 
		}
                SF_INFO(m_logger, "Exiting scap loop... shutting down" );
                SF_INFO(m_logger, "Container Table: " << m_containerCxt->getSize() << " Process Table: " << m_processCxt->getSize() << " NetworkFlow Table: " << m_dfPrcr->getSize() << " Num Records Written: " << m_writer->getNumRecs() );
	}
	catch(sinsp_exception& e)
	{
	    SF_ERROR(m_logger, "Sysdig exception " << e.what());
    	    return 1;
        }catch(avro::Exception& ex) {
            SF_ERROR(m_logger, "Avro Exception! Error: " << ex.what());
    	    return 1;
        }
	return 0;

}


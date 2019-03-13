#include "context.h"
Context::Context(time_t strt, bool fCont, int fDur, bool prefix, string oFile, string sFile, string schFile, string expID ) : procs(PROC_TABLE_SIZE), conts(CONT_TABLE_SIZE), exit(false), filterCont(fCont), 
                      start(strt), fileDuration(fDur), numRecs(0), dfw(NULL), inspector(NULL), hasPrefix(prefix),
                      outputFile(oFile), scapFile(sFile), schemaFile(schFile), exporterID(expID) {
        empkey.hpid = 0; 
        empkey.createTS = 0; 
	procs.set_empty_key(&empkey);
	conts.set_empty_key("0");
        delkey.hpid = 1;
        delkey.createTS = 1;
	procs.set_deleted_key(&delkey);
	conts.set_deleted_key("");
        inspector = new sinsp();
        inspector->set_hostname_and_port_resolution_mode(false);

    }

Context::~Context() {
   clearTables();
   if(inspector != NULL) {
      inspector->close();
      delete inspector;
   }
   if(dfw != NULL) {
      dfw->close();
      delete dfw;
   }


}

int Context::initialize() {

    time_t curTime = 0;
   
    if(start > 0) {
        curTime = time(NULL);
        if(hasPrefix) {
            ofile = outputFile + "." + std::to_string(curTime);
        }else {
            ofile = outputFile + "/" + std::to_string(curTime);
        }
    }else {
        if(hasPrefix) {
            ofile = outputFile;
        } else {
          cout << "When not using the -G option, a file prefix must be set using -p." << endl;
          return 1;
        }
    }
    try {
         sysfSchema  = utils::loadSchema(schemaFile.c_str());
    }catch(avro::Exception& ex) {
         return 1;
    }
    dfw = new avro::DataFileWriter<SysFlow>(ofile.c_str(), sysfSchema, COMPRESS_BLOCK_SIZE, avro::Codec::DEFLATE_CODEC); 
    inspector->open(scapFile);
    header::writeHeader(this);
    return 0;
}

bool Context::checkAndRotateFile()  {
     int fileRotated = false;
     if(start > 0) {
         time_t curTime = time(NULL);
         double duration = difftime(curTime, start);
         if(duration >= fileDuration) {
             if(hasPrefix) {
                 ofile = outputFile + "." + std::to_string(curTime);
             }else {
                 ofile = outputFile + "/" + std::to_string(curTime);
             }
             cout << "Container Table: " << conts.size() <<  " Process Table: " << procs.size() << " Num Records Written: " << numRecs << " New File: " << ofile << endl;
             numRecs = 0;
   	     dfw->close();
             delete dfw; 
             dfw = new avro::DataFileWriter<SysFlow>(ofile.c_str(), sysfSchema, COMPRESS_BLOCK_SIZE, avro::Codec::DEFLATE_CODEC); 
             start = curTime;
	     header::writeHeader(this);
             clearTables();
             fileRotated = true;
         }
     }
     return fileRotated;
}


void Context::clearTables() {
   for(ProcessTable::iterator it = procs.begin(); it != procs.end(); ++it) {
       delete it->second;
   }
   procs.clear();
   for(ContainerTable::iterator it = conts.begin(); it != conts.end(); ++it) {
       delete it->second;
   }
   conts.clear();
}




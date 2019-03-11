#define __STDC_FORMAT_MACROS
#ifdef HAS_CAPTURE
#include <fstream>
#include "driver_config.h"
#endif // HAS_CAPTURE
#include <sinsp.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include "sysflow/sysflow.hh"
#include "avro/ValidSchema.hh"
#include "avro/Compiler.hh"
#include "avro/DataFile.hh"
#include "avro/Encoder.hh"
#include "avro/Decoder.hh"
#include "hashtable.h"
using namespace std;
using namespace sysflow;

SysFlow s_flow;
ProcessFlow s_procFlow;
ProcessTable s_procs(50000);
ContainerTable s_conts(100);
bool s_exit = false;
bool s_filterCont = false;
time_t s_start = 0;
int s_fileDuration = 0;
int s_numRecs = 0;
//ParameterMapping s_params(50);

inline string getUserName(sinsp* inspector, uint32_t uid)
{
    unordered_map<uint32_t, scap_userinfo*>::const_iterator it;
    if(uid == 0xffffffff)
    {
        return string("");
    }

    it = inspector->m_userlist.find(uid);
    if(it == inspector->m_userlist.end())
    {
        return string("");
    }

    return it->second->name;
}

inline string getGroupName(sinsp* inspector, uint32_t gid)
{
    unordered_map<uint32_t, scap_groupinfo*>::const_iterator it;
    if(gid == 0xffffffff)
    {
        return string("");
    }

    it = inspector->m_grouplist.find(gid);
    if(it == inspector->m_grouplist.end())
    {
        return string("");
    }

    return it->second->name;
}

inline bool isInContainer(sinsp_evt* ev) {
    sinsp_threadinfo* ti = ev->get_thread_info();
    return !ti->m_container_id.empty();
}

inline Container*  createContainer(sinsp* inspector, sinsp_evt* ev) {
    sinsp_threadinfo* ti = ev->get_thread_info();
    
    if(ti->m_container_id.empty()) {
        return NULL;
    }

    sinsp_container_info* container = inspector->m_container_manager.get_container(ti->m_container_id);
    if(container == NULL) {
       return NULL;
    }
    Container* cont = new Container();
    cont->name = container->m_name;
    cont->image = container->m_image + "/" + container->m_imagetag;
    cont->id = container->m_id;
    cont->imageid = container->m_imageid;
    cont->type = (ContainerType)container->m_type;
    return cont;
}

inline void writeContainer(avro::DataFileWriter<SysFlow>* dfw, Container* container) {
    s_flow.rec.set_Container(*container);
    s_numRecs++;
    dfw->write(s_flow);
}

inline Container* getContainer(sinsp* inspector, avro::DataFileWriter<SysFlow>* dfw, sinsp_evt* ev) {
    sinsp_threadinfo* ti = ev->get_thread_info();
    
    if(ti->m_container_id.empty()) {
        return NULL;
    }
    ContainerTable::iterator cont = s_conts.find(ti->m_container_id);
    if(cont != s_conts.end()) {
        return cont->second;
    }
    Container* container = createContainer(inspector, ev);
    s_conts[container->id] = container;
    writeContainer(dfw, container);
    return container;
}

inline Process* createProcess(sinsp* inspector, avro::DataFileWriter<SysFlow>* dfw, sinsp_threadinfo* mainthread, sinsp_evt* ev, ActionType actType) {
   
   Process* proc = new Process();
   proc->type = actType;
   proc->ts = ev->get_ts();
   proc->oid.hpid = mainthread->m_pid;
   proc->oid.createTS = mainthread->m_clone_ts;

   sinsp_threadinfo* parent = mainthread->get_parent_thread();

   if(parent != NULL) {
      OID poid;
      poid.createTS = parent->m_clone_ts;
      poid.hpid = parent->m_pid;
      proc->poid.set_OID(poid);
   }
   //std::memcpy(&proc.oid[0], &proc.ts, sizeof(int64_t));
   //std::memcpy(&proc->oid[0], &mainthread->m_clone_ts, sizeof(int64_t));
   //std::memcpy(&proc->oid[8], &proc->hpid, sizeof(int32_t));
   //cout << "Wrote OID" << endl;
   proc->exe = mainthread->m_exepath;
   proc->exeArgs.clear();
   int i = 0;
   for(std::vector<string>::iterator it = mainthread->m_args.begin(); it != mainthread->m_args.end(); ++it) {
       if (i == 0) {
           proc->exeArgs += *it;
       }else {
           proc->exeArgs +=  " " + *it;
       }
       i++;
   }
   //cout << "Wrote exe args" << endl;
    proc->uid = mainthread->m_uid;
    proc->gid = mainthread->m_gid;
   //cout << "Wrote gid/uid " << ti->m_uid << endl;
   proc->userName = getUserName(inspector, mainthread->m_uid);
  // cout << "Wrote username" << endl;
    proc->groupName = getGroupName(inspector, mainthread->m_gid);
    Container* cont = getContainer(inspector, dfw, ev);
    if(cont != NULL) {
       proc->containerId.set_string(cont->id);
    }else {
       proc->containerId.set_null();
    }
    //cout << "Wrote user/groupnames" << endl;
    /*if(mainthread->m_clone_ts != 0) {
       proc->duration = ev->get_ts() - mainthread->m_clone_ts;
    }*/
    //proc->childCount = mainthread->m_nchilds;
    return proc;
}

inline void updateProcess(Process* proc, sinsp* inspector, sinsp_evt* ev, ActionType actType) {
   sinsp_threadinfo* ti = ev->get_thread_info();
   sinsp_threadinfo* mainthread = ti->get_main_thread();
   proc->type = actType;
   proc->ts = ev->get_ts();
   proc->exe = mainthread->m_exepath;
   proc->exeArgs.clear();
   int i = 0;
   for(std::vector<string>::iterator it = mainthread->m_args.begin(); it != mainthread->m_args.end(); ++it) {
       if (i == 0) {
           proc->exeArgs += *it;
       }else {
           proc->exeArgs +=  " " + *it;
       }
       i++;
   }
    proc->uid = mainthread->m_uid;
    proc->gid = mainthread->m_gid;
   //cout << "Wrote gid/uid " << ti->m_uid << endl;
   proc->userName = getUserName(inspector, mainthread->m_uid);
  // cout << "Wrote username" << endl;
   proc->groupName = getGroupName(inspector, mainthread->m_gid);

}



inline void writeProcess(avro::DataFileWriter<SysFlow>* dfw, Process* proc) {
    s_flow.rec.set_Process(*proc);
    s_numRecs++;
    dfw->write(s_flow);
    //avro::encode(*encoder, flow);
}

inline void writeHeader(sinsp* inspector, avro::DataFileWriter<SysFlow>* dfw, string exporterID) {
   SFHeader header;
   header.version = 1000;
   /*char domain[256];
   memset(domain, 0, 256);
   if(getdomainname(domain, 256) ) {
      cout << "Error calling getdomainname for sysflow header. ERROR: " <<  std::strerror(errno) << endl;
      exit(1); 
   }*/
  /* const scap_machine_info* mi = inspector->get_machine_info();
   if(mi != NULL) {
       header.exporter = mi->hostname;
   }*/
   header.exporter = exporterID;
   s_flow.rec.set_SFHeader(header);
   s_numRecs++;
   dfw->write(s_flow);
}





inline Process* getProcess(sinsp* inspector, avro::DataFileWriter<SysFlow>* dfw, sinsp_evt* ev, ActionType actType, bool& created) {
      sinsp_threadinfo* ti = ev->get_thread_info();
      sinsp_threadinfo* mt = ti->get_main_thread();
      OID key;
      key.createTS = mt->m_clone_ts;
      key.hpid = mt->m_pid;
      created = true;
      //std::memcpy(&key[0], &mt->m_clone_ts, sizeof(int64_t));
      //std::memcpy(&key[8], &mt->m_pid, sizeof(int32_t));
      ProcessTable::iterator proc = s_procs.find(&key);
      if(proc != s_procs.end()) {
          created = false;
          return proc->second;
      }
      std::vector<Process*> processes; 
      Process* process = createProcess(inspector, dfw, mt, ev, actType);
      processes.push_back(process);
      mt = mt->get_parent_thread();
      while(mt != NULL) {
	  key.createTS = mt->m_clone_ts;
	  key.hpid = mt->m_pid;
          //std::memcpy(&key[0], &mt->m_clone_ts, sizeof(int64_t));
          //std::memcpy(&key[8], &mt->m_pid, sizeof(int32_t));
          ProcessTable::iterator proc2 = s_procs.find(&key);
          if(proc2 != s_procs.end()) {
	      break;
          }
          Process* parent = createProcess(inspector, dfw, mt, ev, ActionType::REUP);
          processes.push_back(parent);
          mt = mt->get_parent_thread();
      }

      for(vector<Process*>::reverse_iterator it = processes.rbegin(); it != processes.rend(); ++it) {
          s_procs[&(*it)->oid] = (*it);
          writeProcess(dfw, (*it));
      }
      return process;
}

inline void clearTables() {
   for(ProcessTable::iterator it = s_procs.begin(); it != s_procs.end(); ++it) {
       delete it->second;
   }
   s_procs.clear();
   for(ContainerTable::iterator it = s_conts.begin(); it != s_conts.end(); ++it) {
       delete it->second;
   }
   s_conts.clear();
}


inline int64_t getSyscallResult(sinsp_evt* ev) {
      int64_t res = -1;
      if(ev->get_num_params() >= 1) {
	 const ppm_param_info* param = ev->get_param_info(0);
     	 const sinsp_evt_param* p = ev->get_param(0);
         switch(param->type) {
            case PT_PID:
            case PT_ERRNO:
		res = *(int64_t *)p->m_val;
                break;
  	    default:
               cout << "Clone Syscall result not of type pid!! Type: " << param->type <<  " Name: " << param->name <<  endl;
               break;
        }
      }
     /* for(int i = 0; i < ev->get_num_params(); i ++) {
	 string name = ev->get_param_name(i);
	 const ppm_param_info* param = ev->get_param_info(i);
     	 const sinsp_evt_param* p = ev->get_param_value_raw(name.c_str());
         cout << name  << " " << ev->get_param_value_str(name.c_str()) << " " <<  param->type << " " << (uint32_t)param->ninfo <<   endl;
	 if(param->type == PT_PID) {
	    int64_t pid = *(int64_t *)p->m_val;
	    cout << pid << endl;
         }
      }*/
      return res;
}



inline void writeCloneEvent(sinsp* inspector, avro::DataFileWriter<SysFlow>* dfw, sinsp_evt* ev) {
    sinsp_threadinfo* ti = ev->get_thread_info();
    bool created = false;
    Process* proc = getProcess(inspector, dfw, ev, ActionType::CREATED, created);
    s_procFlow.type =  CLONE;
    s_procFlow.ts = ev->get_ts();
    s_procFlow.procOID.hpid = proc->oid.hpid;
    s_procFlow.procOID.createTS = proc->oid.createTS;
    s_procFlow.tid = ti->m_tid;
    s_procFlow.ret = getSyscallResult(ev);
    s_flow.rec.set_ProcessFlow(s_procFlow);
    s_numRecs++;
    dfw->write(s_flow);  
}

inline void writeExitEvent(sinsp* inspector, avro::DataFileWriter<SysFlow>* dfw, sinsp_evt* ev) {
    sinsp_threadinfo* ti = ev->get_thread_info();
    bool created = false;
    Process* proc = getProcess(inspector, dfw, ev, ActionType::REUP, created);
    s_procFlow.type =  EXIT;
    s_procFlow.ts = ev->get_ts();
    s_procFlow.procOID.hpid = proc->oid.hpid;
    s_procFlow.procOID.createTS = proc->oid.createTS;
    s_procFlow.tid = ti->m_tid;
    s_procFlow.ret = getSyscallResult(ev);
    s_flow.rec.set_ProcessFlow(s_procFlow);
    s_numRecs++;
    dfw->write(s_flow);
    // delete the process from the proc table after an exit
    s_procs.erase(&proc->oid);
    delete proc; 

}
inline void writeExecEvent(sinsp* inspector, avro::DataFileWriter<SysFlow>* dfw, sinsp_evt* ev) {
    sinsp_threadinfo* ti = ev->get_thread_info();
    bool created = false;
    Process* proc = getProcess(inspector, dfw, ev, ActionType::CREATED, created);
   /* if(!created) {
      cout << "Exec on an existing process!" << endl;
    }*/
    updateProcess(proc, inspector, ev, ActionType::MODIFIED);
    writeProcess(dfw, proc);

    s_procFlow.type =  EXEC;
    s_procFlow.ts = ev->get_ts();
    s_procFlow.procOID.hpid = proc->oid.hpid;
    s_procFlow.procOID.createTS = proc->oid.createTS;
    s_procFlow.tid = ti->m_tid;
    s_procFlow.ret = getSyscallResult(ev);
    s_flow.rec.set_ProcessFlow(s_procFlow);
    s_numRecs++;
    dfw->write(s_flow);  
}


/*
avro::EncoderPtr createEncoder(string filename) {
    std::auto_ptr<avro::OutputStream> out = avro::fileOutputStream(filename.c_str());
    avro::EncoderPtr e = avro::binaryEncoder();
    e->init(*out);
    return e;
}*/




avro::ValidSchema loadSchema(const char* filename)
{
    avro::ValidSchema result;
    try {
        std::ifstream ifs(filename);
        avro::compileJsonSchema(ifs, result);
     }catch(avro::Exception& ex) {
       cout << "Unable to load schema file from " << filename << " Error: " << ex.what() << endl;
       throw; 
     }
    return result;
}



int runEventLoop(string scapFile, string outputFile, string schemaFile, string exporterID, bool hasPrefix) {
	int32_t res;
	sinsp_evt* ev;
        //OID empkey = { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 , 0, 0 } };
        OID empkey;
        empkey.hpid = 0; 
        empkey.createTS = 0; 
	s_procs.set_empty_key(&empkey);
	s_conts.set_empty_key("0");
        //OID delkey = { { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 , 0, 0 } };
        OID delkey;
        delkey.hpid = 1;
        delkey.createTS = 1;
	s_procs.set_deleted_key(&delkey);
	s_conts.set_deleted_key("");
	cout << "Loading scap file " << scapFile << endl;
       // avro::EncoderPtr encoder;
        //avro::EncoderPtr encoder = createEncoder(outputFile);
        //std::auto_ptr<avro::OutputStream> out = avro::fileOutputStream(outputFile.c_str());
        //avro::EncoderPtr encoder = avro::binaryEncoder();
        //encoder->init(*out);
        string ofile;
        time_t curTime = 0;
     
        if(s_start > 0) {
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


        avro::ValidSchema sysfSchema;
        try {
            sysfSchema  = loadSchema(schemaFile.c_str());
        }catch(avro::Exception& ex) {
            return 1;
        }
        avro::DataFileWriter<SysFlow>* dfw = new avro::DataFileWriter<SysFlow>(ofile.c_str(), sysfSchema, 80000, avro::Codec::DEFLATE_CODEC); 
	sinsp* inspector = new sinsp();
        inspector->set_hostname_and_port_resolution_mode(false);
	try
	{
		//inspector->set_buffer_format(sinsp_evt::PF_NORMAL);
		inspector->open(scapFile);
		writeHeader(inspector, dfw, exporterID);
		while(true) 
		{
			res = inspector->next(&ev);
			//cout << "Retrieved a scap event... RES: " << res << endl;
			if(res == SCAP_TIMEOUT)
			{
				/*	if(ev != NULL && ev->is_filtered_out())
				{
					//
					// The event has been dropped by the filtering system.
					// Give the chisels a chance to run their timeout logic.
					//
					chisels_do_timeout(ev);
				}*/
                                if(s_exit) {
                                    break;
                                }

                                if(s_start > 0) {
                                    curTime = time(NULL);
                                    double duration = difftime(curTime, s_start);
                                    if(duration >= s_fileDuration) {
                                         if(hasPrefix) {
                                            ofile = outputFile + "." + std::to_string(curTime);
                                         }else {
                                            ofile = outputFile + "/" + std::to_string(curTime);
                                         }
                                        cout << "Container Table: " << s_conts.size() <<  " Process Table: " << s_procs.size() << " Num Records Written: " << s_numRecs << " New File: " << ofile << endl;
					s_numRecs = 0;
   				        dfw->close();
                                        delete dfw; 
                                        dfw = new avro::DataFileWriter<SysFlow>(ofile.c_str(), sysfSchema, 80000, avro::Codec::DEFLATE_CODEC); 
					s_start = curTime;
		                        writeHeader(inspector, dfw, exporterID);
                                        clearTables();
                                    }
                                }
				continue;
			}
			else if(res == SCAP_EOF)
			{
				//handle_end_of_file(print_progress, formatter);
				break;
			}
			else if(res != SCAP_SUCCESS)
			{
				//
				// Event read error.
				// Notify the chisels that we're exiting, and then die with an error.
				//
				//handle_end_of_file(print_progress, formatter);
				cerr << "res = " << res << endl;
				throw sinsp_exception(inspector->getlasterr().c_str());
			}
			switch(ev->get_type()) {
                          case PPME_SYSCALL_EXECVE_8_E:
                          case PPME_SYSCALL_EXECVE_13_E:
                          case PPME_SYSCALL_EXECVE_14_E:
                          case PPME_SYSCALL_EXECVE_15_E:
                          case PPME_SYSCALL_EXECVE_16_E:
                          case PPME_SYSCALL_EXECVE_17_E:
                          case PPME_SYSCALL_EXECVE_18_E:
                          case PPME_SYSCALL_EXECVE_19_E:
                          {
                              break;
                          }
                          case PPME_SYSCALL_EXECVE_8_X:
			  case PPME_SYSCALL_EXECVE_13_X:
			  case PPME_SYSCALL_EXECVE_14_X:
			  case PPME_SYSCALL_EXECVE_15_X:
			  case PPME_SYSCALL_EXECVE_16_X:
			  case PPME_SYSCALL_EXECVE_17_X:
			  case PPME_SYSCALL_EXECVE_18_X:
			  case PPME_SYSCALL_EXECVE_19_X:
                          {
                              if(s_filterCont && !isInContainer(ev)) {
                                    break;
                              }
			      writeExecEvent(inspector, dfw, ev);
                              break;
                          }
			  case PPME_SYSCALL_CLONE_16_X:
			  case PPME_SYSCALL_CLONE_17_X:
			  case PPME_SYSCALL_CLONE_20_X:
                          {
                              if(s_filterCont && !isInContainer(ev)) {
                                    break;
                              }
			      writeCloneEvent(inspector, dfw, ev);
                              break;
                          }
			  case PPME_PROCEXIT_E:
			  case PPME_PROCEXIT_X:
			  case PPME_PROCEXIT_1_E:
			  case PPME_PROCEXIT_1_X:
			  {
                              if(s_filterCont && !isInContainer(ev)) {
                                    break;
                              }
			      writeExitEvent(inspector, dfw, ev);		
			      break;
                          }
			
                       } 
         	       if(s_exit) {
                           break;
                       }
                       if(s_start > 0) {
                             curTime = time(NULL);
                             double duration = difftime(curTime, s_start);
                             if(duration >= s_fileDuration) {
                                 if(hasPrefix) {
                                     ofile = outputFile + "." + std::to_string(curTime);
                                 }else {
                                     ofile = outputFile + "/" + std::to_string(curTime);
                                 }
                                 cout << "Container Table: " << s_conts.size() << " Process Table: " << s_procs.size() << " Num Records Written: " << s_numRecs << " New File: " << ofile << endl;
			  	 s_numRecs = 0;
   		                 dfw->close();
                                 delete dfw; 
                                 dfw = new avro::DataFileWriter<SysFlow>(ofile.c_str(), sysfSchema, 80000, avro::Codec::DEFLATE_CODEC); 
			         s_start = curTime;
		                 writeHeader(inspector, dfw, exporterID);
                                 clearTables();
                               }
                       }
		}
                cout << "Exiting scap loop... shutting down" << endl;
                cout << "Container Table: " << s_conts.size() << " Process Table: " << s_procs.size() << " Num Records Written: " << s_numRecs << endl;
                clearTables();
		inspector->close();
                //(encoder)->flush();
		dfw->close();
                delete dfw;
	}
	catch(sinsp_exception& e)
	{
	    cerr << "Sysdig exception " << e.what() << endl;
    	    return 1;
        }catch(avro::Exception& ex) {
            cout << "Avro Exception! Error: " << ex.what() << endl;
    	    return 1;
        }
	delete inspector;
	return 0;
}

void signal_handler(int i) {
   cout << "Received Signal " << i << endl;
   s_exit = true;   
}

enum STR2INT_ERROR { SUCC, OFLOW, UFLOW, INCONVERTIBLE };

STR2INT_ERROR str2int (int &i, char const *s, int base = 0)
{
    char *end;
    long  l;
    errno = 0;
    l = strtol(s, &end, base);
    if ((errno == ERANGE && l == LONG_MAX) || l > INT_MAX) {
        return OFLOW;
    }
    if ((errno == ERANGE && l == LONG_MIN) || l < INT_MIN) {
        return UFLOW;
    }
    if (*s == '\0' || *end != '\0') {
        return INCONVERTIBLE;
    }
    i = l;
    return SUCC;
}



int main( int argc, char** argv )
{
	string scapFile;
	string outputDir;
        string prefix ="";
        string exporterID = "";
        char* duration;
	char c;
        struct sigaction sigIntHandler;
        string schemaFile = "/usr/local/sysflow/conf/SysFlow.avsc";

        sigIntHandler.sa_handler = signal_handler;
        sigemptyset(&sigIntHandler.sa_mask);
        sigIntHandler.sa_flags = 0;

        sigaction(SIGINT, &sigIntHandler, NULL);


	while ((c = getopt (argc, argv, "lcr:w:G:s:p:e:")) != -1)
    	{
		switch (c)
      		{
      			case 'e':
        			exporterID = optarg;
        			break;
      			case 'p':
        			prefix = optarg;
        			break;
      			case 'r':
        			scapFile = optarg;
        			break;
      			case 'w':
        			outputDir = optarg;
        			break;
                        case 'G': 
                                duration = optarg;
                                if(str2int(s_fileDuration, duration, 10)) {
                                    cout << "Unable to parse " << duration << endl;
                                    exit(1);
                                }
                                if(s_fileDuration < 1) {
				    cout << "File duration must be higher than 0" << endl;
                                    exit(1);
                                }
                                s_start = time(NULL);
                                break;
			case 'l':
				scapFile = "";
				cout << "Live Capture initiated.." << endl;
				break;
                        case 'c': 
                               s_filterCont = true;
                               break;
                        case 's':
                               schemaFile = optarg;
                               break;
      			case '?':
        			if (optopt == 'r' || optopt == 'm')
          				fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        			else if (isprint (optopt))
          				fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        			else
          				fprintf (stderr,
                   			"Unknown option character `\\x%x'.\n",
                   			optopt);
        			return 1;
      			default:
        			abort ();
      		}
	}

        if(exporterID.empty()) {
           cout << "Exporter ID must be set with -e.  Exiting.." << endl;
           return 1;
       }

        if(!prefix.empty()) {
            outputDir += prefix;
        }
       
	return runEventLoop(scapFile, outputDir, schemaFile, exporterID, !prefix.empty());
}


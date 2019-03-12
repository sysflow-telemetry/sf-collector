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
#include "sysflow/sysflow.hh"
#include "avro/ValidSchema.hh"
#include "avro/Compiler.hh"
#include "avro/DataFile.hh"
#include "avro/Encoder.hh"
#include "avro/Decoder.hh"


#include "hashtables.h"
#include "utils.h"
#include "container.h"
#include "process.h"
#include "header.h"
#include "context.h"
#include "processflow.h"
using namespace std;
using namespace sysflow;

Context* s_cxt = NULL;


int runEventLoop(Context* cxt, string scapFile, string outputFile, string schemaFile, string exporterID, bool hasPrefix) {
	int32_t res;
	sinsp_evt* ev;
        //OID empkey = { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 , 0, 0 } };
        OID empkey;
        empkey.hpid = 0; 
        empkey.createTS = 0; 
	cxt->procs.set_empty_key(&empkey);
	cxt->conts.set_empty_key("0");
        //OID delkey = { { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 , 0, 0 } };
        OID delkey;
        delkey.hpid = 1;
        delkey.createTS = 1;
	cxt->procs.set_deleted_key(&delkey);
	cxt->conts.set_deleted_key("");
	cout << "Loading scap file " << scapFile << endl;
       // avro::EncoderPtr encoder;
        //avro::EncoderPtr encoder = createEncoder(outputFile);
        //std::auto_ptr<avro::OutputStream> out = avro::fileOutputStream(outputFile.c_str());
        //avro::EncoderPtr encoder = avro::binaryEncoder();
        //encoder->init(*out);
        string ofile;
        time_t curTime = 0;
     
        if(cxt->start > 0) {
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
        cxt->dfw = new avro::DataFileWriter<SysFlow>(ofile.c_str(), sysfSchema, 80000, avro::Codec::DEFLATE_CODEC); 
	cxt->inspector = new sinsp();
        cxt->inspector->set_hostname_and_port_resolution_mode(false);
	try
	{
		//inspector->set_buffer_format(sinsp_evt::PF_NORMAL);
		cxt->inspector->open(scapFile);
		writeHeader(cxt, exporterID);
		while(true) 
		{
			res = cxt->inspector->next(&ev);
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
                                if(cxt->exit) {
                                    break;
                                }

                                if(cxt->start > 0) {
                                    curTime = time(NULL);
                                    double duration = difftime(curTime, cxt->start);
                                    if(duration >= cxt->fileDuration) {
                                         if(hasPrefix) {
                                            ofile = outputFile + "." + std::to_string(curTime);
                                         }else {
                                            ofile = outputFile + "/" + std::to_string(curTime);
                                         }
                                        cout << "Container Table: " << cxt->conts.size() <<  " Process Table: " << cxt->procs.size() << " Num Records Written: " << cxt->numRecs << " New File: " << ofile << endl;
					cxt->numRecs = 0;
   				        cxt->dfw->close();
                                        delete cxt->dfw; 
                                        cxt->dfw = new avro::DataFileWriter<SysFlow>(ofile.c_str(), sysfSchema, 80000, avro::Codec::DEFLATE_CODEC); 
					cxt->start = curTime;
		                        writeHeader(cxt, exporterID);
                                        clearTables(cxt);
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
				throw sinsp_exception(cxt->inspector->getlasterr().c_str());
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
                              if(cxt->filterCont && !isInContainer(ev)) {
                                    break;
                              }
			      writeExecEvent(cxt, ev);
                              break;
                          }
			  case PPME_SYSCALL_CLONE_16_X:
			  case PPME_SYSCALL_CLONE_17_X:
			  case PPME_SYSCALL_CLONE_20_X:
                          {
                              if(cxt->filterCont && !isInContainer(ev)) {
                                    break;
                              }
			      writeCloneEvent(cxt, ev);
                              break;
                          }
			  case PPME_PROCEXIT_E:
			  case PPME_PROCEXIT_X:
			  case PPME_PROCEXIT_1_E:
			  case PPME_PROCEXIT_1_X:
			  {
                              if(cxt->filterCont && !isInContainer(ev)) {
                                    break;
                              }
			      writeExitEvent(cxt, ev);		
			      break;
                          }
			
                       } 
         	       if(cxt->exit) {
                           break;
                       }
                       if(cxt->start > 0) {
                             curTime = time(NULL);
                             double duration = difftime(curTime, cxt->start);
                             if(duration >= cxt->fileDuration) {
                                 if(hasPrefix) {
                                     ofile = outputFile + "." + std::to_string(curTime);
                                 }else {
                                     ofile = outputFile + "/" + std::to_string(curTime);
                                 }
                                 cout << "Container Table: " << cxt->conts.size() << " Process Table: " << cxt->procs.size() << " Num Records Written: " << cxt->numRecs << " New File: " << ofile << endl;
			  	 cxt->numRecs = 0;
   		                 cxt->dfw->close();
                                 delete cxt->dfw; 
                                 cxt->dfw = new avro::DataFileWriter<SysFlow>(ofile.c_str(), sysfSchema, 80000, avro::Codec::DEFLATE_CODEC); 
			         cxt->start = curTime;
		                 writeHeader(cxt, exporterID);
                                 clearTables(cxt);
                               }
                       }
		}
                cout << "Exiting scap loop... shutting down" << endl;
                cout << "Container Table: " << cxt->conts.size() << " Process Table: " << cxt->procs.size() << " Num Records Written: " << cxt->numRecs << endl;
                clearTables(cxt);
		cxt->inspector->close();
                //(encoder)->flush();
		cxt->dfw->close();
                delete cxt->dfw;
	}
	catch(sinsp_exception& e)
	{
	    cerr << "Sysdig exception " << e.what() << endl;
    	    return 1;
        }catch(avro::Exception& ex) {
            cout << "Avro Exception! Error: " << ex.what() << endl;
    	    return 1;
        }
	delete cxt->inspector;
	return 0;
}

void signal_handler(int i) {
   cout << "Received Signal " << i << endl;
   s_cxt->exit = true;   
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
        s_cxt = new Context();
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
                                if(str2int(s_cxt->fileDuration, duration, 10)) {
                                    cout << "Unable to parse " << duration << endl;
                                    exit(1);
                                }
                                if(s_cxt->fileDuration < 1) {
				    cout << "File duration must be higher than 0" << endl;
                                    exit(1);
                                }
                                s_cxt->start = time(NULL);
                                break;
			case 'l':
				scapFile = "";
				cout << "Live Capture initiated.." << endl;
				break;
                        case 'c': 
                               s_cxt->filterCont = true;
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
       
	return runEventLoop(s_cxt, scapFile, outputDir, schemaFile, exporterID, !prefix.empty());
}


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

#include "syscall_defs.h"
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


int runEventLoop(Context* cxt) {
	int32_t res;
	sinsp_evt* ev;
        //OID empkey = { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 , 0, 0 } };
       // avro::EncoderPtr encoder;
        //avro::EncoderPtr encoder = createEncoder(outputFile);
        //std::auto_ptr<avro::OutputStream> out = avro::fileOutputStream(outputFile.c_str());
        //avro::EncoderPtr encoder = avro::binaryEncoder();
        //encoder->init(*out);

	try
	{
                cxt->initialize();
		//inspector->set_buffer_format(sinsp_evt::PF_NORMAL);
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
				cxt->checkAndRotateFile();
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
         	        if(cxt->exit) {
                           break;
                        }
                        cxt->checkAndRotateFile();
                        if(cxt->filterCont && !utils::isInContainer(ev)) {
                              continue;
                        }
			switch(ev->get_type()) {
                          SF_EXECVE_ENTER
                          {
                              break;
                          }
                          SF_EXECVE_EXIT
                          {
			      processflow::writeExecEvent(cxt, ev);
                              break;
                          }
                          SF_CLONE_EXIT
                          {
			      processflow::writeCloneEvent(cxt, ev);
                              break;
                          }
                          SF_PROCEXIT_E_X	
			  {
			      processflow::writeExitEvent(cxt, ev);		
			      break;
                          }
			
                       } 
		}
                cout << "Exiting scap loop... shutting down" << endl;
                cout << "Container Table: " << cxt->conts.size() << " Process Table: " << cxt->procs.size() << " Num Records Written: " << cxt->numRecs << endl;
	}
	catch(sinsp_exception& e)
	{
	    cerr << "Sysdig exception " << e.what() << endl;
    	    return 1;
        }catch(avro::Exception& ex) {
            cout << "Avro Exception! Error: " << ex.what() << endl;
    	    return 1;
        }
	delete cxt;
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
	string scapFile = "";
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
        bool filterCont = false;
        int fileDuration = 0;
        time_t start = 0;

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
                                if(str2int(fileDuration, duration, 10)) {
                                    cout << "Unable to parse " << duration << endl;
                                    exit(1);
                                }
                                if(fileDuration < 1) {
				    cout << "File duration must be higher than 0" << endl;
                                    exit(1);
                                }
                                start = time(NULL);
                                break;
			case 'l':
				scapFile = "";
				cout << "Live Capture initiated.." << endl;
				break;
                        case 'c': 
                               filterCont = true;
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

        if(prefix.empty() && start == 0) {
            cout << "When not using the -G option, a file prefix must be set using -p." << endl;
            return 1;
        }

        s_cxt = new Context(start, filterCont, fileDuration, !prefix.empty(), outputDir, scapFile, schemaFile, exporterID);
       
	return runEventLoop(s_cxt);
}


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
#include "sysflowprocessor.h"

using namespace std;

using namespace sysflowprocessor;

SysFlowProcessor* s_prc = NULL;



void signal_handler(int i) {
   cout << "Received Signal " << i << endl;
   s_prc->exit();   
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

        if(prefix.empty() && fileDuration == 0) {
            cout << "When not using the -G option, a file prefix must be set using -p." << endl;
            return 1;
        }

        SysFlowContext* cxt = new SysFlowContext(filterCont, fileDuration, !prefix.empty(), outputDir, scapFile, schemaFile, exporterID);
        s_prc = new SysFlowProcessor(cxt);
        int ret =  s_prc->run();
        delete s_prc;
        return ret;
}


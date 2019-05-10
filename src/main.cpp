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
#include "logger.h"

using namespace std;

using namespace sysflowprocessor;

SysFlowProcessor* s_prc = NULL;



void signal_handler(int i) {
   s_prc->exit();   
}


int str2int (int &i, char const *s, int base = 0)
{
    char *end;
    long  l;
    errno = 0;
    l = strtol(s, &end, base);
    if ((errno == ERANGE && l == LONG_MAX) || l > INT_MAX) {
        return -1;
    }
    if ((errno == ERANGE && l == LONG_MIN) || l < INT_MIN) {
        return -1;
    }
    if (*s == '\0' || *end != '\0') {
        return -1;
    }
    i = l;
    return 0;
}


static void usage(std::string name)
{
    std::cerr << "Usage: " << name << " [-h] [-G <interval>] [-s <schema file>] [-c] [-e <exporterID>] [-r <scap file>] [-l <log conf file>] -w <file name/dir>\n"
              << "Options:\n"
              << "\t-h\t\t\tShow this help message\n"
              <<  "\t-w file name/dir\t(required) The file or directory to which sysflow records are written\n"
              <<  "\t\t\t\tIf a directory is specified (using a trailing slash), file name will be an epoch timestamp\n"
              <<  "\t\t\t\tIf -G is specified, then the file name specified will have an epoch timestamp appended to it\n"
              << "\t-e exporterID\t\tA globally unique ID representing the host or VM being monitored which is stored in the sysflow dumpfile header\n"
              << "\t\t\t\tIf -e not set, the hostname of the CURRENT machine is used,  which may not be accurate for reading offline scap files\n"
              << "\t-G interval(in secs)\tRotates the dumpfile specified in -w every interval seconds and appends epoch timestamp to file name\n"
              << "\t-r scap file\t\tThe scap file to be read and dumped as sysflow format at the file specified by -w\n"
              << "\t\t\t\tIf this option is not specified, a live capture is assumed\n"
              << "\t-s schema file\t\tThe sysflow avro schema file (.avsc) used for schema validation (default: /usr/local/sysflow/conf/SysFlow.avsc)\n"
              << "\t-f filter\t\tSysdig style filtering string to filter scap. Must be surrounded by quotes\n"
              << "\t-c\t\t\tSimple, fast filter to allow only container-related events to be dumped\n"
              << "\t-l log conf file\tLocation of log4cxx properties configuration file. (default: /usr/local/sysflow/conf/log4cxx.properties)\n"
              << std::endl;
}

static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("sysflow.main"));


int main( int argc, char** argv )
{
	string scapFile = "";
	string outputDir;
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
        string filter = "";
        bool help = false;
        string logProps =  "/usr/local/sysflow/conf/log4cxx.properties";

        sigaction(SIGINT, &sigIntHandler, NULL);


	while ((c = getopt (argc, argv, "hcr:w:G:s:e:l:")) != -1)
    	{
		switch (c)
      		{
      			case 'e':
        			exporterID = optarg;
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
                        case 'c': 
                               filterCont = true;
                               break;
                        case 's':
                               schemaFile = optarg;
                               break;
                        case 'f':
                               filter = optarg;
                               break;
                        case 'l':
                               logProps = optarg;
                               break;
                        case 'h':
                               help = true;
                               break;
      			case '?':
        			if (optopt == 'r' || optopt == 's' || optopt == 'f' || optopt == 'w' || optopt == 'G' || optopt == 'l')
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

        if(help) {
           usage(argv[0]);
           return 0;

        }
        if(outputDir.empty()) {
           usage(argv[0]);
           return 1;
        }

        try
        {
            if (!logProps.empty())
            {
                PropertyConfigurator::configure(logProps.c_str());
            }
            else
            {
                BasicConfigurator::configure();
            }
            LOG4CXX_DEBUG(logger, "Starting sysporter..")
            SysFlowContext* cxt = new SysFlowContext(filterCont, fileDuration, outputDir, scapFile, schemaFile, exporterID, filter);
            s_prc = new SysFlowProcessor(cxt);
            int ret =  s_prc->run();
            delete s_prc;
            return ret;
 
        }
        catch(Exception& ex)
        {
            cerr << ex.what() << endl;
            return 1;
        }
}


#define __STDC_FORMAT_MACROS
#include <fstream>
#include <stdio.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include "sysflow/sysflow.hh"
#include "avro/ValidSchema.hh"
#include "avro/Compiler.hh"
#include "avro/DataFile.hh"
#include "avro/Encoder.hh"
#include "avro/Decoder.hh"
#include <arpa/inet.h>
#include "op_flags.h"
using namespace std;
using namespace sysflow;

#define HEADER 0
#define CONT 1 
#define PROC 2
#define PROC_FLOW 3
#define NET_FLOW 4

Process proc;
ProcessFlow pf;
SysFlow flow;
SFHeader header;
Container cont;
NetworkFlow netflow;

const char* Events[] = {"CLONE", "EXEC", "EXIT"};


avro::ValidSchema loadSchema(const char* filename)
{
    avro::ValidSchema result;
    try {
        std::ifstream ifs(filename);
        avro::compileJsonSchema(ifs, result);
    }catch(avro::Exception & ex) {
         cerr << "Unable to load schema from " << filename << endl;
         exit(1);
    }
    return result;
}


void printNetFlow(NetworkFlow netflow) {
    struct in_addr srcIP;
    struct in_addr dstIP;
    srcIP.s_addr = netflow.sip;
    dstIP.s_addr = netflow.dip;
    string opFlags = "";
    opFlags +=  ((netflow.opFlags & OP_NF_ACCEPT) ?  "A" : " ");
    opFlags +=  ((netflow.opFlags & OP_NF_CONNECT) ?  "C" : " ");
    opFlags +=  ((netflow.opFlags & OP_NF_SEND) ?  "S" : " ");
    opFlags +=  ((netflow.opFlags & OP_NF_RECV) ?  "R" : " ");
    opFlags +=  ((netflow.opFlags & OP_NF_CLOSE) ?  "C" : " ");
    opFlags +=  ((netflow.opFlags & OP_NF_DELEGATE) ?  "D" : " ");
    opFlags +=  ((netflow.opFlags & OP_NF_INHERIT) ?  "I" : " ");
    opFlags +=  ((netflow.opFlags & OP_NF_TRUNCATE) ?  "T" : " ");
    opFlags +=  ((netflow.opFlags & OP_NF_FINAL) ?  "F" : " ");

    cout << "NETFLOW " << netflow.startTs << " " << netflow.endTs << " " <<  opFlags << " SIP: " << inet_ntoa(srcIP) << " " << " DIP: " << inet_ntoa(dstIP) << " SPORT: " << netflow.sport << " DPORT: " << netflow.dport << " PROTO: " << netflow.proto << " WBytes: " << netflow.numWBytes << " RBytes: " << netflow.numRBytes << " WOps: " << netflow.numWOps << " ROps: " << netflow.numROps << endl;
}



int runEventLoop(string sysFile, string schemaFile) {
	int32_t res;
	cout << "Loading sys file " << sysFile << endl;
       // avro::EncoderPtr encoder;
        //avro::EncoderPtr encoder = createEncoder(outputFile);
        //std::auto_ptr<avro::OutputStream> out = avro::fileOutputStream(outputFile.c_str());
        //avro::EncoderPtr encoder = avro::binaryEncoder();
        //encoder->init(*out);
        avro::ValidSchema sysfSchema = loadSchema(schemaFile.c_str());
        avro::DataFileReader<SysFlow> dfr(sysFile.c_str(), sysfSchema);
        int i = 0;
        while (dfr.read(flow)) {
            switch(flow.rec.idx()) {
              case HEADER:
              {
                  header = flow.rec.get_SFHeader();
                  cout << "Version: " << header.version  << " Exporter: " << header.exporter << endl;
                  break;
              }
              case PROC:
              {
                 proc = flow.rec.get_Process();
		 cout << "PROC " <<  proc.ts << " " << proc.type << " " << proc.exe << " " <<  proc.exeArgs << " " << proc.oid.hpid << " " <<  proc.userName << " " << proc.oid.createTS;
                 if(!proc.poid.is_null()) {
                   cout << " Parent: " << proc.poid.get_OID().hpid << " " << proc.poid.get_OID().createTS;
                 }
                 if(!proc.containerId.is_null()) {
		    cout << " Container ID: " << proc.containerId.get_string() << endl;
                 }
                 //cout << string(proc.oid.begin(), proc.oid.end());
                 cout << endl;
                 break;
              }
              case PROC_FLOW:
              {
		pf = flow.rec.get_ProcessFlow();
                cout << "PROC_FLOW " << pf.ts << " " << Events[pf.type] << " " <<  " " << pf.ret << " OID: " << pf.procOID.hpid << " " << pf.procOID.createTS << endl;
                break;
              }
              case CONT: 
              {
                cont = flow.rec.get_Container();
		cout << "CONT Name: " << cont.name << " ID: " << cont.id << " Image: " << cont.image << " Image ID: " << cont.imageid << " Type: " << cont.type << endl;
		break;
              }
              case NET_FLOW:
              {
                  netflow = flow.rec.get_NetworkFlow();
                  printNetFlow(netflow);
                  break;
              }
              default:
              {
                cout << "No sysflow union type has object mapping to index " << flow.rec.idx() << endl;
              }
            }
            i++;
        }
	cout << "Number of records: " << i << endl;
	return 0;
}



int main( int argc, char** argv )
{
	string sysFile;
        string schemaFile = "/usr/local/sysflow/conf/SysFlow.avsc";
	char c;
	while ((c = getopt (argc, argv, "lr:w:s:")) != -1)
    	{
		switch (c)
      		{
      			case 'r':
        			sysFile = optarg;
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
	return runEventLoop(sysFile, schemaFile);
}


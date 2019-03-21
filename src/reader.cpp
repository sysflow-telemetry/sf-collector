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
#include "datatypes.h"
#include <time.h>
using namespace std;
using namespace sysflow;

#define HEADER 0
#define CONT 1 
#define PROC 2
#define PROC_FLOW 3
#define NET_FLOW 4
#define NANO_TO_SECS 1000000000

Process proc;
ProcessFlow pf;
SysFlow flow;
SFHeader header;
Container cont;
NetworkFlow netflow;

ProcessTable s_procs;

bool s_printProc = false;
bool s_printCont = false;

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

   ProcessTable::iterator it = s_procs.find(&(netflow.procOID));
   if(it == s_procs.end()) {
       cout << "Uh Oh! Can't find process for netflow!! " << endl;
       cout << "NETFLOW " << netflow.startTs << " " << netflow.endTs << " " <<  opFlags << " SIP: " << inet_ntoa(srcIP) << " " << " DIP: " << inet_ntoa(dstIP) << " SPORT: " << netflow.sport << " DPORT: " << netflow.dport << " PROTO: " << netflow.proto << " WBytes: " << netflow.numWBytes << " RBytes: " << netflow.numRBytes << " WOps: " << netflow.numWOps << " ROps: " << netflow.numROps << " " << netflow.procOID.hpid << " " << netflow.procOID.createTS <<  endl;
  } else {
       time_t startTs = ((time_t)(netflow.startTs/NANO_TO_SECS));
       time_t endTs = ((time_t)(netflow.endTs/NANO_TO_SECS));
       char startTime[100];
       char endTime[100];
       strftime(startTime, 99, "%x %X %Z", localtime(&startTs));
       strftime(endTime, 99, "%x %X %Z", localtime(&endTs));
       string container = "";
       if(!it->second->containerId.is_null()) {
                  container = it->second->containerId.get_string();
       }
       cout << it->second->exe << " " << container << " " << it->second->oid.hpid << " " << startTime << " " << endTime << " " <<  opFlags << " SIP: " << inet_ntoa(srcIP) << " " << " DIP: " << inet_ntoa(dstIP) << " SPORT: " << netflow.sport << " DPORT: " << netflow.dport << " PROTO: " << netflow.proto << " WBytes: " << netflow.numWBytes << " RBytes: " << netflow.numRBytes << " WOps: " << netflow.numWOps << " ROps: " << netflow.numROps << " " <<  it->second->exe << " " << it->second->exeArgs << endl;

 }
}

Process*  createProcess(Process proc) {
   Process* p = new Process();
   p->type = proc.type;
   p->ts = proc.ts;
   p->oid.hpid = proc.oid.hpid;
   p->oid.createTS = proc.oid.createTS;

   if(!proc.poid.is_null()) {
       OID poid = proc.poid.get_OID();
       p->poid.set_OID(poid);
   }
   p->exe = proc.exe;
   p->exeArgs = proc.exeArgs;
   /*int i = 0;
   for(std::vector<string>::iterator it = mainthread->m_args.begin(); it != mainthread->m_args.end(); ++it) {
       if (i == 0) {
           proc->exeArgs += *it;
       }else {
           proc->exeArgs +=  " " + *it;
       }
       i++;
   }*/
   //cout << "Wrote exe args" << endl;
    p->uid = proc.uid;
    p->gid = proc.gid;
   //cout << "Wrote gid/uid " << ti->m_uid << endl;
   p->userName = proc.userName;
  // cout << "Wrote username" << endl;
    p->groupName = proc.groupName;
    if(!proc.containerId.is_null()) {
       p->containerId.set_string(proc.containerId.get_string());
    }else {
       p->containerId.set_null();
    }
    //cout << "Wrote user/groupnames" << endl;
    /*if(mainthread->m_clone_ts != 0) {
       proc->duration = ev->get_ts() - mainthread->m_clone_ts;
    }*/
    //proc->childCount = mainthread->m_nchilds;
    return p;

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
                 if(s_printProc) {
		     cout << "PROC " << proc.oid.hpid << " " << proc.oid.createTS << " " <<   proc.ts << " " << proc.type << " " << proc.exe << " " <<  proc.exeArgs << " " << proc.oid.hpid << " " <<  proc.userName << " " << proc.oid.createTS;
                     if(!proc.poid.is_null()) {
                         cout << " Parent: " << proc.poid.get_OID().hpid << " " << proc.poid.get_OID().createTS;
                     }
                     if(!proc.containerId.is_null()) {
		         cout << " Container ID: " << proc.containerId.get_string() << endl;
                     }else {
                         cout << endl;
                     }
                 }
                 ProcessTable::iterator it = s_procs.find(&(proc.oid));
                 if(it != s_procs.end() && proc.type != ActionType::MODIFIED) {
                     cout << "Uh oh! Process " << it->second->exe << " already in the process table PID: " << it->second->oid.hpid << " Create TS: " << it->second->oid.createTS << endl;
                 }else {
                     Process* p = createProcess(proc);
                     //cout << "Adding process " << proc.oid.hpid << " " << proc.oid.createTS << " to table " << endl;
                     //cout << "Copied process " << p->oid.hpid << " " << p->oid.createTS << " to table " << endl;
                     if(it != s_procs.end()) {
                         Process* oldP = it->second;
                         s_procs.erase(&(oldP->oid));
                         delete oldP;
                     }
                     s_procs[&(p->oid)] = p;
                     //cout << "Number procs in hashtable: " << s_procs.size() << endl;
                 }
          

                 //cout << string(proc.oid.begin(), proc.oid.end());
                 break;
              }
              case PROC_FLOW:
              {
		pf = flow.rec.get_ProcessFlow();
                time_t timestamp = ((time_t)(pf.ts/NANO_TO_SECS));
                char times[100];
                strftime(times, 99, "%x %X %Z", localtime(&timestamp));
                ProcessTable::iterator it = s_procs.find(&(pf.procOID));
                if(it == s_procs.end()) {
                   cout << "Can't find process for process flow!  shouldn't happen!!" << endl;
                   cout << "PROC_FLOW " << times << " " << Events[pf.type] << " " <<  " " << pf.ret << " OID: " << pf.procOID.hpid << " " << pf.procOID.createTS << endl;
                }  else {
                   
                   string container = "";
                  if(!it->second->containerId.is_null()) {
                       container = it->second->containerId.get_string();
                  }
           

                   cout << it->second->exe << " " << container << " " << it->second->oid.hpid << " " <<  times << " " << Events[pf.type] << " " <<  " " << pf.ret <<  " " << pf.procOID.createTS << " " <<  it->second->exe << " " << it->second->exeArgs << endl;

                }
               if(pf.type == 2) { // exit
                   if(it != s_procs.end()) {
                        delete it->second;
                        s_procs.erase(&(pf.procOID));
                   }
                }

                break;
              }
              case CONT: 
              {
                cont = flow.rec.get_Container();
                if(s_printCont) {
		    cout << "CONT Name: " << cont.name << " ID: " << cont.id << " Image: " << cont.image << " Image ID: " << cont.imageid << " Type: " << cont.type << endl;
                }
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
        OID empkey;
        OID delkey;
        empkey.hpid = 0; 
        empkey.createTS = 0; 
        s_procs.set_empty_key(&empkey);
        delkey.hpid = 1;
        delkey.createTS = 1;
        s_procs.set_deleted_key(&delkey);
	while ((c = getopt (argc, argv, "lr:w:s:pc")) != -1)
    	{
		switch (c)
      		{
      			case 'r':
        			sysFile = optarg;
        			break;
                        case 's':
                               schemaFile = optarg;
                               break;
                        case 'p':
                               s_printProc = true;
                        case 'c':
                               s_printCont = true;

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


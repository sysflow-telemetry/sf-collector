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
#define BUFFERSIZE 20
#include "b64/encode.h"
using namespace std;
using namespace sysflow;

#define HEADER 0
#define CONT 1 
#define PROC 2
#define FILE_ 3
#define PROC_FLOW 4
#define NET_FLOW 5
#define FILE_FLOW 6

#define NANO_TO_SECS 1000000000

Process proc;
ProcessFlow pf;
SysFlow flow;
SFHeader header;
Container cont;
File file;
NetworkFlow netflow;
FileFlow fileflow;

typedef google::dense_hash_map<OID*, Process*, MurmurHasher<OID*>, eqoidptr> PTable;
typedef google::dense_hash_map<string, File*, MurmurHasher<string>, eqstr> FTable;
PTable s_procs;
FTable s_files;

bool s_quiet = false;
bool s_keepProcOnExit = false;

const char* Events[] = {"", "CLONE", "EXEC", "",  "EXIT"};


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
void printFileFlow(FileFlow fileflow) {
    string opFlags = "";
    opFlags +=  ((fileflow.opFlags & OP_OPEN) ?  "O" : " ");
    opFlags +=  ((fileflow.opFlags & OP_ACCEPT) ?  "A" : " ");
    opFlags +=  ((netflow.opFlags & OP_CONNECT) ?  "C" : " ");
    opFlags +=  ((netflow.opFlags & OP_WRITE_SEND) ?  "W" : " ");
    opFlags +=  ((netflow.opFlags & OP_READ_RECV) ?  "R" : " ");
    opFlags +=  ((netflow.opFlags & OP_CLOSE) ?  "C" : " ");
    opFlags +=  ((netflow.opFlags & OP_TRUNCATE) ?  "T" : " ");
    opFlags +=  ((netflow.opFlags & OP_DIGEST) ?  "D" : " ");

    time_t startTs = ((time_t)(netflow.ts/NANO_TO_SECS));
    time_t endTs = ((time_t)(netflow.endTs/NANO_TO_SECS));
    char startTime[100];
    char endTime[100];
    strftime(startTime, 99, "%x %X %Z", localtime(&startTs));
     strftime(endTime, 99, "%x %X %Z", localtime(&endTs));

    string key(fileflow.fileOID.begin(), fileflow.fileOID.end());
    FTable::iterator fi = s_files.find(key);
    base64::encoder enc(20);
    char b64encoded[60];
    int len = enc.encode(key.c_str(), key.size(),  b64encoded);
    string b64enc(b64encoded, len);
    len = enc.encode_end(b64encoded);
    b64enc += string(b64encoded, len);
    if(fi == s_files.end()) {
        cout << "Uh Oh! Can't find process for fileflow!! " << endl;
        cout << "FILEFLOW " << startTime << " " << endTime << " " <<  opFlags << " TID: " << fileflow.tid << " FD: " << fileflow.fd << " WBytes: " << fileflow.numWSendBytes << " RBytes: " << fileflow.numRRecvBytes << " WOps: " << fileflow.numWSendOps << " ROps: " << fileflow.numRRecvOps << " " << fileflow.procOID.hpid << " " << fileflow.procOID.createTS <<  b64enc << endl;

      }


   PTable::iterator it = s_procs.find(&(fileflow.procOID));
   if(it == s_procs.end()) {
       cout << "Uh Oh! Can't find process for fileflow!! " << endl;
       cout << "FILEFLOW " << startTime << " " << endTime << " " <<  opFlags << " TID: " << fileflow.tid << " FD: " << fileflow.fd << " WBytes: " << fileflow.numWSendBytes << " RBytes: " << fileflow.numRRecvBytes << " WOps: " << fileflow.numWSendOps << " ROps: " << fileflow.numRRecvOps << " " << fileflow.procOID.hpid << " " << fileflow.procOID.createTS << b64enc << endl;
  } else {
       string container = "";
       if(!it->second->containerId.is_null()) {
                  container = it->second->containerId.get_string();
       }
       //cout << netflow.sip << "\t" << netflow.dip << endl;
       cout << it->second->exe << " " << container << " " << it->second->oid.hpid << " " << startTime << " " << endTime << " " <<  opFlags << " Resource: " << fi->second->restype << " PATH: " << fi->second->path << " FD: " << fileflow.fd << " TID: " << fileflow.tid <<  " WBytes: " << fileflow.numWSendBytes << " RBytes: " << fileflow.numRRecvBytes << " WOps: " << fileflow.numWSendOps << " ROps: " << fileflow.numRRecvOps << " " <<  it->second->exe << " " << it->second->exeArgs << b64enc << endl;

 }
}


void printNetFlow(NetworkFlow netflow) {
    struct in_addr srcIP;
    struct in_addr dstIP;
    srcIP.s_addr = netflow.sip;
    dstIP.s_addr = netflow.dip;
    string opFlags = "";
    opFlags +=  ((netflow.opFlags & OP_ACCEPT) ?  "A" : " ");
    opFlags +=  ((netflow.opFlags & OP_CONNECT) ?  "C" : " ");
    opFlags +=  ((netflow.opFlags & OP_WRITE_SEND) ?  "S" : " ");
    opFlags +=  ((netflow.opFlags & OP_READ_RECV) ?  "R" : " ");
    opFlags +=  ((netflow.opFlags & OP_CLOSE) ?  "C" : " ");
    opFlags +=  ((netflow.opFlags & OP_TRUNCATE) ?  "T" : " ");
    opFlags +=  ((netflow.opFlags & OP_DIGEST) ?  "D" : " ");

    string srcIPStr = string(inet_ntoa(srcIP));
    string dstIPStr = string(inet_ntoa(dstIP));
    time_t startTs = ((time_t)(netflow.ts/NANO_TO_SECS));
    time_t endTs = ((time_t)(netflow.endTs/NANO_TO_SECS));
    char startTime[100];
    char endTime[100];
    strftime(startTime, 99, "%x %X %Z", localtime(&startTs));
     strftime(endTime, 99, "%x %X %Z", localtime(&endTs));

   PTable::iterator it = s_procs.find(&(netflow.procOID));
   if(it == s_procs.end()) {
       cout << "Uh Oh! Can't find process for netflow!! " << endl;
       cout << "NETFLOW " << startTime << " " << endTime << " " <<  opFlags << " SIP: " << srcIPStr << " " << " DIP: " << dstIPStr << " SPORT: " << netflow.sport << " DPORT: " << netflow.dport << " PROTO: " << netflow.proto << " WBytes: " << netflow.numWSendBytes << " RBytes: " << netflow.numRRecvBytes << " WOps: " << netflow.numWSendOps << " ROps: " << netflow.numRRecvOps << " " << netflow.procOID.hpid << " " << netflow.procOID.createTS <<  endl;
  } else {
       string container = "";
       if(!it->second->containerId.is_null()) {
                  container = it->second->containerId.get_string();
       }
       //cout << netflow.sip << "\t" << netflow.dip << endl;
       cout << it->second->exe << " " << container << " " << it->second->oid.hpid << " " << startTime << " " << endTime << " " <<  opFlags << " SIP: " << srcIPStr << " " << " DIP: " << dstIPStr << " SPORT: " << netflow.sport << " DPORT: " << netflow.dport << " PROTO: " << netflow.proto << " WBytes: " << netflow.numWSendBytes << " RBytes: " << netflow.numRRecvBytes << " WOps: " << netflow.numWSendOps << " ROps: " << netflow.numRRecvOps << " " <<  it->second->exe << " " << it->second->exeArgs << endl;

 }
}

File* createFile(File file) {
   File* f = new File();
   f->state = file.state;
   f->ts = file.ts;
   f->restype = file.restype;
   f->path = file.path;
   if(!file.containerId.is_null()) {
       f->containerId.set_string(file.containerId.get_string());
    }else {
       f->containerId.set_null();
    }
    return f;
}


Process*  createProcess(Process proc) {
   Process* p = new Process();
   p->state = proc.state;
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
                 if(!s_quiet) {
		     cout << "PROC " << proc.oid.hpid << " " << proc.oid.createTS << " " <<   proc.ts << " " << proc.state << " " << proc.exe << " " <<  proc.exeArgs << " " << proc.oid.hpid << " " <<  proc.userName << " " << proc.oid.createTS << " TTY: " << proc.tty;
                     if(!proc.poid.is_null()) {
                         cout << " Parent: " << proc.poid.get_OID().hpid << " " << proc.poid.get_OID().createTS;
                     }
                     if(!proc.containerId.is_null()) {
		         cout << " Container ID: " << proc.containerId.get_string() << endl;
                     }else {
                         cout << endl;
                     }
                 }
                 PTable::iterator it = s_procs.find(&(proc.oid));
                 if(it != s_procs.end() && proc.state != SFObjectState::MODIFIED) {
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
              case FILE_:
              {
                  file = flow.rec.get_File();
                  File* f = createFile(file);
                  string key(file.oid.begin(), file.oid.end());
                  FTable::iterator it = s_files.find(key);
                  cout << "FILE: " << f->path << " " << f->ts << " " << f->state << " " << f->restype << " " << key << endl;
                  //std::cout.write(&file.oid[0], 20);
                  if(it != s_files.end()) {
                      cout << "Uh oh!  File:  " << f->path << " already exists in the sysflow file" << endl;
                  }
                  s_files[key] = f;
                  break;
              }
              case PROC_FLOW:
              {
		pf = flow.rec.get_ProcessFlow();
                time_t timestamp = ((time_t)(pf.ts/NANO_TO_SECS));
                char times[100];
                strftime(times, 99, "%x %X %Z", localtime(&timestamp));
                PTable::iterator it = s_procs.find(&(pf.procOID));
                if(it == s_procs.end()) {
                   cout << "Can't find process for process flow!  shouldn't happen!!" << endl;
                   cout << "PROC_FLOW " << times << " " << Events[pf.opFlags] << " " <<  " " << pf.ret << " OID: " << pf.procOID.hpid << " " << pf.procOID.createTS << endl;
                }  else {
                   
                   string container = "";
                  if(!it->second->containerId.is_null()) {
                       container = it->second->containerId.get_string();
                  }
           

                   cout << it->second->exe << " " << container << " " << it->second->oid.hpid << " " <<  times << " " << Events[pf.opFlags] << " " <<  " " << pf.ret <<  " " << pf.procOID.createTS << " " <<  it->second->exe << " " << it->second->exeArgs << endl;

                }
               if(!s_keepProcOnExit && pf.opFlags == OP_EXIT) { // exit
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
                if(!s_quiet) {
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
              case FILE_FLOW:
              {
                  fileflow = flow.rec.get_FileFlow();
                  printFileFlow(fileflow);
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
        s_files.set_empty_key("-1");
        s_files.set_deleted_key("-2");
	while ((c = getopt (argc, argv, "lr:w:s:qk")) != -1)
    	{
		switch (c)
      		{
      			case 'r':
        			sysFile = optarg;
        			break;
                        case 's':
                               schemaFile = optarg;
                               break;
                        case 'q':
                               s_quiet = true;
                               break;
                        case 'k':
                               s_keepProcOnExit = true;
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


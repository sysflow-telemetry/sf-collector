#ifndef _SF_CONTEXT_
#define _SF_CONTEXT_

#include "avro/ValidSchema.hh"
#include "avro/Compiler.hh"
#include "avro/DataFile.hh"
#include "avro/Encoder.hh"
#include "avro/Decoder.hh"
#include <string>

#include <time.h>
#include <sinsp.h>

#include "utils.h"
#include "header.h"
#include "hashtables.h"
#include "networkflow.h"
#include "sysflow/sysflow.hh"

#define CONT_TABLE_SIZE 100
#define PROC_TABLE_SIZE 50000
#define NF_TABLE_SIZE 50000
#define COMPRESS_BLOCK_SIZE 80000

using namespace std;
using namespace sysflow;
class Context {
public:
    SysFlow flow;
    ProcessFlow procFlow;
    ProcessTable procs;
    ContainerTable conts;
    NetworkFlowTable netflows;
    bool exit;
    bool filterCont;
    time_t start;
    int fileDuration;
    int numRecs;
    avro::DataFileWriter<SysFlow>* dfw; 
    sinsp* inspector;
    bool hasPrefix;
    string outputFile;
    string scapFile;
    string schemaFile;
    string exporterID;
    string ofile;
    Context(time_t start, bool fCont, int fDur, bool prefix, string oFile, string sFile, string schFile, string exporterID); 
    virtual ~Context();
    int initialize();
    bool checkAndRotateFile();
    void clearTables();
private:
    OID empkey;
    OID delkey;
    avro::ValidSchema sysfSchema;
    networkflow::NFKey nfdelkey;
    networkflow::NFKey nfemptykey;

};

#endif

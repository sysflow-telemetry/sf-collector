#ifndef _SF_CONTEXT_
#define _SF_CONTEXT_

#include "avro/ValidSchema.hh"
#include "avro/Compiler.hh"
#include "avro/DataFile.hh"
#include "avro/Encoder.hh"
#include "avro/Decoder.hh"


#include <time.h>
#include <sinsp.h>

#include "hashtables.h"
#include "sysflow/sysflow.hh"
using namespace sysflow;
class Context {
public:
    SysFlow flow;
    ProcessFlow procFlow;
    ProcessTable procs;
    ContainerTable conts;
    bool exit;
    bool filterCont;
    time_t start;
    int fileDuration;
    int numRecs;
    avro::DataFileWriter<SysFlow>* dfw; 
    sinsp* inspector; 
    Context() : procs(50000), conts(100), exit(false), filterCont(false), 
                      start(0), fileDuration(0), numRecs(0), dfw(NULL), inspector(NULL) {

    }
};

#endif

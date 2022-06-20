
#include "sysflowlibs.hpp"

void process_sysflow(sysflow::SFHeader* header, sysflow::Container* cont, sysflow::Process* proc, sysflow::File* f1, sysflow::File* f2, sysflow::SysFlow* rec) {
  printf("Hello!!\n");
}

int main(int argc, char **argv) {
    SysFlowConfig* config = sysflowlibscpp::InitializeSysFlowConfig();
    strncpy(config->scapInputPath, "./tcp-client-server.scap", 255);
    config->callback = process_sysflow;
    sysflowlibscpp::SysFlowDriver *driver = new sysflowlibscpp::SysFlowDriver(config);
    driver->run();
}
#include "sysflow/enums.hh"
#include "sysflowlibs.hpp"
#include <csignal>
#include <functional>
#include <stdio.h>
#include <stdlib.h>

namespace {
std::function<void()> shutdown_handler;
void signal_handler(int /*i*/) { shutdown_handler(); }
} // namespace

void printFile(sysflow::File *file) {
  printf("File: Type %d, Path %s\n", file->restype, file->path.c_str());
}

void process_sysflow(sysflow::SFHeader *header, sysflow::Container *cont,
                     sysflow::Process *proc, sysflow::File *f1,
                     sysflow::File *f2, sysflow::SysFlow *flow) {
  printf("****************************************************************\n");
  printf("Header: Exporter %s, IP %s, File name %s\n", header->exporter.c_str(),
         header->ip.c_str(), header->filename.c_str());

  if (cont != nullptr) {
    printf("Container: Id %s, Name %s, Image %s, ImageId %s, Type, %d\n",
           cont->id.c_str(), cont->name.c_str(), cont->image.c_str(),
           cont->imageid.c_str(), (int)cont->type);
  }

  printf("Process: PID %lu Creation Time, %lu, Exe %s, Exe Args %s, User Name "
         "%s, Group Name %s, TTY %d\n",
         proc->oid.hpid, proc->oid.createTS, proc->exe.c_str(),
         proc->exeArgs.c_str(), proc->userName.c_str(), proc->groupName.c_str(),
         proc->tty);

  if (f1 != nullptr) {
    printFile(f1);
  }

  if (f2 != nullptr) {
    printFile(f2);
  }

  switch (flow->rec.idx()) {
  case SF_PROC_EVT: {
    sysflow::ProcessEvent pe = flow->rec.get_ProcessEvent();
    printf("Proc Evt: TID %lu, OpFlags %d, Ret %d\n", pe.tid, pe.opFlags,
           pe.ret);
    break;
  }
  case SF_NET_FLOW: {
    sysflow::NetworkFlow nf = flow->rec.get_NetworkFlow();
    printf("Network Flow: TID %lu, OpFlags: %d, SIP %d, SPort %d, DIP %d, "
           "DPort %d\n",
           nf.tid, nf.opFlags, nf.sip, nf.sport, nf.dip, nf.dport);
    break;
  }
  case SF_FILE_FLOW: {
    sysflow::FileFlow ff = flow->rec.get_FileFlow();
    printf("File Flow: TID %lu, OpFlags: %d, OpenFlags %d, FD %d\n", ff.tid,
           ff.opFlags, ff.openFlags, ff.fd);
    break;
  }
  case SF_FILE_EVT: {
    sysflow::FileEvent fe = flow->rec.get_FileEvent();
    printf("File Flow: TID %lu, OpFlags: %d, Ret %d\n", fe.tid, fe.opFlags,
           fe.ret);
    break;
  }
  case SF_NET_EVT: {
    break;
  }
  case SF_PROC_FLOW: {
    printf("Proc Flow received\n");
    break;
  }
  default: {
    printf("Received unexpected flow type %lu\n", flow->rec.idx());
    break;
  }
  }

  printf("****************************************************************\n");
}

int main(int argc, char **argv) {
  // configure event collection (using defaults)
  SysFlowConfig *config = sysflowlibscpp::InitializeSysFlowConfig();
  config->callback = process_sysflow;
  sysflowlibscpp::SysFlowDriver *driver =
      new sysflowlibscpp::SysFlowDriver(config);

  // register signal handlers to stop event collection
  shutdown_handler = [&]() -> void { driver->exit(); };
  std::signal(SIGINT, signal_handler);
  std::signal(SIGTERM, signal_handler);

  // start event collection
  driver->run();

  // clean up resources
  delete driver;
  delete config;
}

#ifndef _SF_CONT_
#define _SF_CONT_

#include <string>

#include <sinsp.h>
#include "sysflow/sysflow.hh"
#include "sysflowcontext.h"
#include "sysflowwriter.h"
#include "datatypes.h"

#define CONT_TABLE_SIZE 100
using namespace std;
using namespace sysflow;
namespace container {
    class ContainerContext {
       private:
          ContainerTable m_containers;
          SysFlowContext* m_cxt;
          SysFlowWriter* m_writer;
          Container*  createContainer(sinsp_evt* ev); 
       public:
          ContainerContext(SysFlowContext* cxt, SysFlowWriter* writer);
          virtual ~ContainerContext();
          Container* getContainer(sinsp_evt* ev);
          void clearContainers();
          inline int getSize() {
              return m_containers.size();
          }
    };
}
#endif

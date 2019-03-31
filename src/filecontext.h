#ifndef _SF_FILE_
#define _SF_FILE_
#include "sysflow/sysflow.hh"
#include "datatypes.h"
#include "sysflowwriter.h"
#include "containercontext.h"
using namespace sysflow;
namespace file {
    class FileContext {
        private:
            SysFlowWriter* m_writer;
            FileTable m_files;
            container::ContainerContext* m_containerCxt;
        public:
            FileContext(container::ContainerContext* containerCxt, SysFlowWriter* writer);
            virtual ~FileContext();
            FileObj* getFile(sinsp_evt* ev, SFObjectState state, bool& created);
            FileObj* createFile(sinsp_evt* ev, SFObjectState state, string key);
            void clearFiles();
            inline int getSize() {
                return m_files.size();
            }
     };
}



#endif

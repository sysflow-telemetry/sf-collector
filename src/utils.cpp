#include "utils.h"
#include "sysflowcontext.h"
#include "datatypes.h"
static NFKey s_nfdelkey;
static NFKey s_nfemptykey;
static bool s_nfkeysinit = false;

void initKeys() {
   s_nfdelkey.ip1 = 1;
   s_nfdelkey.ip2 = 1;
   s_nfdelkey.port1 = 1;
   s_nfdelkey.port2 = 1;
   s_nfemptykey.ip1 = 1;
   s_nfemptykey.ip2 = 0;
   s_nfemptykey.port1 = 1;
   s_nfemptykey.port2 = 1;
}

NFKey* utils::getNFEmptyKey() {
    if(!s_nfkeysinit) {
         initKeys();
     }
     return &s_nfemptykey;
}

NFKey* utils::getNFDelKey() {
    if(!s_nfkeysinit) {
         initKeys();
     }
     return &s_nfdelkey;
}

string utils::getUserName(SysFlowContext* cxt, uint32_t uid)
{
    unordered_map<uint32_t, scap_userinfo*>::const_iterator it;
    if(uid == 0xffffffff)
    {
        return string("");
    }

    it = cxt->getInspector()->m_userlist.find(uid);
    if(it == cxt->getInspector()->m_userlist.end())
    {
        return string("");
    }

    return it->second->name;
}

string utils::getGroupName(SysFlowContext* cxt, uint32_t gid)
{
    unordered_map<uint32_t, scap_groupinfo*>::const_iterator it;
    if(gid == 0xffffffff)
    {
        return string("");
    }

    it = cxt->getInspector()->m_grouplist.find(gid);
    if(it == cxt->getInspector()->m_grouplist.end())
    {
        return string("");
    }

    return it->second->name;
}
bool utils::isInContainer(sinsp_evt* ev) {
    sinsp_threadinfo* ti = ev->get_thread_info();
    return !ti->m_container_id.empty();
}


time_t utils::getExportTime(SysFlowContext* cxt) {
    time_t now = utils::getCurrentTime(cxt);
    struct tm exportTM = *localtime( &now);
    exportTM.tm_sec += cxt->getNFExportInterval();   // add 30 seconds to the time
    return mktime( &exportTM);      // normalize iti
}


int64_t utils::getSyscallResult(sinsp_evt* ev) {
      int64_t res = -1;
      if(ev->get_num_params() >= 1) {
	 const ppm_param_info* param = ev->get_param_info(0);
     	 const sinsp_evt_param* p = ev->get_param(0);
         switch(param->type) {
            case PT_PID:
            case PT_ERRNO:
            case PT_INT64:
            case PT_INT32:
		res = *(int64_t *)p->m_val;
                break;
  	    default:
               cout << "Clone Syscall result not of type pid!! Type: " << param->type <<  " Name: " << param->name <<  endl;
               break;
        }
      }
     /* for(int i = 0; i < ev->get_num_params(); i ++) {
	 string name = ev->get_param_name(i);
	 const ppm_param_info* param = ev->get_param_info(i);
     	 const sinsp_evt_param* p = ev->get_param_value_raw(name.c_str());
         cout << name  << " " << ev->get_param_value_str(name.c_str()) << " " <<  param->type << " " << (uint32_t)param->ninfo <<   endl;
	 if(param->type == PT_PID) {
	    int64_t pid = *(int64_t *)p->m_val;
	    cout << pid << endl;
         }
      }*/
      return res;
}


avro::ValidSchema utils::loadSchema(const char* filename)
{
    avro::ValidSchema result;
    try {
        std::ifstream ifs(filename);
        avro::compileJsonSchema(ifs, result);
     }catch(avro::Exception& ex) {
       cout << "Unable to load schema file from " << filename << " Error: " << ex.what() << endl;
       throw; 
     }
    return result;
}

#include "utils.h"
string getUserName(Context* cxt, uint32_t uid)
{
    unordered_map<uint32_t, scap_userinfo*>::const_iterator it;
    if(uid == 0xffffffff)
    {
        return string("");
    }

    it = cxt->inspector->m_userlist.find(uid);
    if(it == cxt->inspector->m_userlist.end())
    {
        return string("");
    }

    return it->second->name;
}

string getGroupName(Context* cxt, uint32_t gid)
{
    unordered_map<uint32_t, scap_groupinfo*>::const_iterator it;
    if(gid == 0xffffffff)
    {
        return string("");
    }

    it = cxt->inspector->m_grouplist.find(gid);
    if(it == cxt->inspector->m_grouplist.end())
    {
        return string("");
    }

    return it->second->name;
}
bool isInContainer(sinsp_evt* ev) {
    sinsp_threadinfo* ti = ev->get_thread_info();
    return !ti->m_container_id.empty();
}

void clearTables(Context* cxt) {
   for(ProcessTable::iterator it = cxt->procs.begin(); it != cxt->procs.end(); ++it) {
       delete it->second;
   }
   cxt->procs.clear();
   for(ContainerTable::iterator it = cxt->conts.begin(); it != cxt->conts.end(); ++it) {
       delete it->second;
   }
   cxt->conts.clear();
}

int64_t getSyscallResult(sinsp_evt* ev) {
      int64_t res = -1;
      if(ev->get_num_params() >= 1) {
	 const ppm_param_info* param = ev->get_param_info(0);
     	 const sinsp_evt_param* p = ev->get_param(0);
         switch(param->type) {
            case PT_PID:
            case PT_ERRNO:
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


avro::ValidSchema loadSchema(const char* filename)
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

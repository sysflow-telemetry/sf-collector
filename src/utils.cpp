#include "utils.h"
#include "sysflowcontext.h"
#include "datatypes.h"
static NFKey s_nfdelkey;
static NFKey s_nfemptykey;
static bool s_keysinit = false;

static OID s_oiddelkey;
static OID s_oidemptykey;



void initKeys() {
   s_nfdelkey.ip1 = 1;
   s_nfdelkey.ip2 = 1;
   s_nfdelkey.port1 = 1;
   s_nfdelkey.port2 = 1;
   s_nfemptykey.ip1 = 1;
   s_nfemptykey.ip2 = 0;
   s_nfemptykey.port1 = 1;
   s_nfemptykey.port2 = 1;
   s_oidemptykey.hpid = 2; 
   s_oidemptykey.createTS = 2;
   s_oiddelkey.hpid = 1; 
   s_oiddelkey.createTS = 1;
   s_keysinit = true;
}


void  utils::generateFOID(string key, FOID* foid) {
    SHA1((const unsigned char*)key.c_str(), key.size(), foid->begin());
}

NFKey* utils::getNFEmptyKey() {
    if(!s_keysinit) {
         initKeys();
     }
     return &s_nfemptykey;
}

NFKey* utils::getNFDelKey() {
    if(!s_keysinit) {
         initKeys();
     }
     return &s_nfdelkey;
}

OID* utils::getOIDEmptyKey() {
    if(!s_keysinit) {
         initKeys();
     }
     return &s_oidemptykey;
}

OID* utils::getOIDDelKey() {
    if(!s_keysinit) {
         initKeys();
     }
     return &s_oiddelkey;
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

string  utils::getPath(sinsp_evt* ev, string paraName) {
    //sinsp_evt_param* param = ev->get_param_value_raw(paraName.c_str());
    int numParams = ev->get_num_params(); 
    string path;
    for(int i = 0; i < numParams; i++) {
        const ppm_param_info* param = ev->get_param_info(i);
        string name = ev->get_param_name(i);
        if(paraName.compare(name) != 0) {
            continue;
        } 
     	const sinsp_evt_param* p = ev->get_param(i);
        if(param->type == PT_FSPATH || param->type == PT_CHARBUF) {
            path = string(p->m_val, p->m_len);
            sanitize_string(path);   
        }
        break;
    }
    return path;
}

int64_t utils::getFD(sinsp_evt* ev, string paraName) {
    int numParams = ev->get_num_params(); 
    int64_t fd = -1;
    for(int i = 0; i < numParams; i++) {
        const ppm_param_info* param = ev->get_param_info(i);
        string name = ev->get_param_name(i);
        if(paraName.compare(name) != 0) {
            continue;
        } 
     	const sinsp_evt_param* p = ev->get_param(i);
        if(param->type == PT_FD) {
            assert(p->m_len == sizeof(int64_t));
            fd = ((int64_t)*(int64_t*)p->m_val);
        }
        break;
    }
    return fd;
}



string utils::getAbsolutePath(sinsp_threadinfo* ti, int64_t dirfd, string fileName) {
    fs::path p(fileName);
    cout << "The path is " << p << " File name " << fileName <<  " relative? " << p.is_relative() << endl;
    if(fileName.empty() || p.is_relative()) {
        cout << "getting in here..." << endl;                
        fs::path tmp;
        string cwd = ti->get_cwd(); 
        if(dirfd == PPM_AT_FDCWD) {
            cout << "Inside PPM_AT_FDCWD..." << endl; 
            if(cwd.empty()) {
                return p.string();
            }
            tmp = ti->get_cwd();
        }else {
            //string dirfd = ev->get_param_value_str("dirfd");
            sinsp_fdinfo_t * fdinfo = ti->get_fd(dirfd);
            assert(fdinfo != NULL);
            tmp = fdinfo->m_name;             
            cout << "Got fddir at... " << tmp << endl;                
        }
        tmp /= fileName;
        cout << "Before canonicalization: " << tmp << endl;
        p = fs::weakly_canonical(tmp);
        cout << "The canonicalized file is " << p << endl;               
    } else {
        p = fs::weakly_canonical(p);
    }
     
    return p.string();
}

string utils::getAbsolutePath(sinsp_threadinfo* ti, string fileName) {
    fs::path p(fileName);
    cout << "The path is " << p << " File name " << fileName <<  " relative? " << p.is_relative() << endl;
    if(fileName.empty() || p.is_relative()) {
        cout << "getting in here..." << endl;                
        fs::path tmp; 
        string cwd = ti->get_cwd();
        if(!cwd.empty()) {
            tmp = cwd;
            tmp /= fileName;
            cout << "Before canonicalization: " << tmp << endl;
            p = fs::weakly_canonical(tmp);
            cout << "The canonicalized file is " << p << endl; 
        }              
    } else {
        p = fs::weakly_canonical(p);
    }
    return p.string();
}





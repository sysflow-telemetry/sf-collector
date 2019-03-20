#ifndef __HASHER__
#define __HASHER__
#include "MurmurHash3.h"

#include <google/dense_hash_map>
#include <set>
#include "sysflow/sysflow.hh"

using namespace std;
using namespace sysflow;
//typedef boost::array<uint8_t, 16> OID;
struct NFKey {
   uint32_t ip1;
   uint16_t port1;
   uint32_t ip2;
   uint16_t port2;
};
   
class NetFlowObj {
    public:
       time_t exportTime;
       time_t lastUpdate;
       NetworkFlow netflow;
};

// simple hash adapter for types without pointers
template<typename T> 
struct MurmurHasher {
    size_t operator()(const T& t) const {
        size_t hash;
        MurmurHash3_x86_32(&t, sizeof(t), 0, &hash);
        return hash;
    }    
};

/*template<> 
struct MurmurHasher<OID*> {
    size_t operator()(const OID* t) const {
        size_t hash;
        MurmurHash3_x86_32(t->begin(), t->size(), 0, &hash);
        return hash;
    }    
};*/
template<> 
struct MurmurHasher<OID*> {
    size_t operator()(const OID* t) const {
        size_t hash;
        MurmurHash3_x86_32((void*)t, sizeof(OID), 0, &hash);
        return hash;
    }    
};

/*struct eqoid
{
  bool operator()(const OID* s1, const OID* s2) const
  {
    return (*s1 == *s2);
  }
};*/
struct eqoid
{
  bool operator()(const OID* s1, const OID* s2) const
  {
    return (s1->hpid == s2->hpid && s1->createTS == s2->createTS);
  }
};
// specialization for strings
template<> 
struct MurmurHasher<string> {
    size_t operator()(const string& t) const {
        size_t hash;
        MurmurHash3_x86_32(t.c_str(), t.size(), 0, &hash);
        return hash;
    }    
};

struct eqstr
{
  bool operator()(const string& s1, const string& s2) const
  {
    return (s1.compare(s2) == 0);
  }
};

template<> 
struct MurmurHasher<NFKey> {
    size_t operator()(const NFKey& t) const {
        size_t hash;
        MurmurHash3_x86_32((void*)&t, sizeof(NFKey), 0, &hash);
        return hash;
    }    
};

struct eqnfkey {
  bool operator()(const NFKey& n1, const NFKey& n2) const
  {
    return (n1.ip1 == n2.ip1 && n1.ip2 == n2.ip2 && 
           n1.port1 == n2.port1 && n1.port2 == n2.port2);
  }
};

struct eqnfobj {
  bool operator()(const NetFlowObj* nf1, const NetFlowObj* nf2) {
      return (nf1->exportTime <= nf2->exportTime);  
 }


};



typedef google::dense_hash_map<OID*, Process*, MurmurHasher<OID*>, eqoid> ProcessTable;
typedef google::dense_hash_map<int, string> ParameterMapping;
typedef google::dense_hash_map<string, Container*, MurmurHasher<string>, eqstr> ContainerTable;
//typedef google::dense_hash_map<OID*, ProcessFlow*, MurmurHasher<OID*>, eqoid> ProcessFlowTable;
typedef google::dense_hash_map<NFKey, NetFlowObj*, MurmurHasher<NFKey>, eqnfkey> NetworkFlowTable;

typedef set<NetFlowObj*, eqnfobj>  NetworkFlowSet;
#endif

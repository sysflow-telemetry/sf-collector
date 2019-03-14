#ifndef __HASHER__
#define __HASHER__
#include "MurmurHash3.h"

#include <google/dense_hash_map>

#include "sysflow/sysflow.hh"
#include "networkflow.h"

using namespace sysflow;
//typedef boost::array<uint8_t, 16> OID;

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
struct MurmurHasher<networkflow::NFKey> {
    size_t operator()(const networkflow::NFKey& t) const {
        size_t hash;
        MurmurHash3_x86_32((void*)&t, sizeof(networkflow::NFKey), 0, &hash);
        return hash;
    }    
};

struct eqnfkey {
  bool operator()(const networkflow::NFKey& n1, const networkflow::NFKey& n2) const
  {
    return (n1.ip1 == n2.ip1 && n1.ip2 == n2.ip2 && 
           n1.port1 == n2.port1 && n1.port2 == n2.port2);
  }
};



typedef google::dense_hash_map<OID*, Process*, MurmurHasher<OID*>, eqoid> ProcessTable;
typedef google::dense_hash_map<int, string> ParameterMapping;
typedef google::dense_hash_map<string, Container*, MurmurHasher<string>, eqstr> ContainerTable;
//typedef google::dense_hash_map<OID*, ProcessFlow*, MurmurHasher<OID*>, eqoid> ProcessFlowTable;
typedef google::dense_hash_map<networkflow::NFKey, networkflow::NetFlowObj*, MurmurHasher<networkflow::NFKey>, eqnfkey> NetworkFlowTable;

#endif

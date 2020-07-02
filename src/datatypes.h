/** Copyright (C) 2019 IBM Corporation.
 *
 * Authors:
 * Frederico Araujo <frederico.araujo@ibm.com>
 * Teryl Taylor <terylt@ibm.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 **/

#ifndef __HASHER__
#define __HASHER__
#include "MurmurHash3.h"
#include "sysflow.h"
#include "utils.h"
#include <google/dense_hash_map>
#include <google/dense_hash_set>
#include <set>
#include <list>

using sysflow::Container;
using sysflow::FileFlow;
using sysflow::NetworkFlow;
using sysflow::ProcessFlow;
using sysflow::OID;
using sysflow::Process;

struct NFKey {
  uint64_t tid;
  uint32_t ip1;
  uint16_t port1;
  uint32_t ip2;
  uint16_t port2;
  uint32_t fd;
};

class OIDObj {
public:
  time_t exportTime;
  OID oid;
  explicit OIDObj(OID o) {
    oid.hpid = o.hpid;
    oid.createTS = o.createTS;
  }
};

class DataFlowObj {
public:
  time_t exportTime;
  time_t lastUpdate;
  bool isNetworkFlow;
  explicit DataFlowObj(bool inf)
      : exportTime(0), lastUpdate(0), isNetworkFlow(inf) {}
};

class NetFlowObj : public DataFlowObj {
public:
  NetworkFlow netflow;
  bool operator==(const NetFlowObj &nfo) {
    if (exportTime != nfo.exportTime) {
      return false;
    }

    return (netflow.procOID.createTS == nfo.netflow.procOID.createTS &&
            netflow.procOID.hpid == nfo.netflow.procOID.hpid &&
            netflow.sip == nfo.netflow.sip && netflow.dip == nfo.netflow.dip &&
            netflow.sport == nfo.netflow.sport &&
            netflow.dport == nfo.netflow.dport &&
            netflow.proto == nfo.netflow.proto &&
            netflow.ts == nfo.netflow.ts && netflow.tid == nfo.netflow.tid &&
            netflow.fd == nfo.netflow.fd);
  }
  NetFlowObj() : DataFlowObj(true) {}
};

class FileFlowObj : public DataFlowObj {
public:
  FileFlow fileflow;
  string filekey;
  string flowkey;
  bool operator==(const FileFlowObj &ffo) {
    if (exportTime != ffo.exportTime) {
      return false;
    }
    return (flowkey.compare(ffo.flowkey) == 0);
  }
  FileFlowObj() : DataFlowObj(false) {}
};

class ProcessFlowObj : public DataFlowObj {
public:
  ProcessFlow procflow;
  bool operator==(const ProcessFlowObj &pfo) {
    if (exportTime != pfo.exportTime) {
      return false;
    }
    return (procflow.procOID.createTS == pfo.procflow.procOID.createTS && procflow.procOID.hpid == pfo.procflow.procOID.hpid);
  }
  ProcessFlowObj() : DataFlowObj(false) {}
};

// simple hash adapter for types without pointers
template <typename T> struct MurmurHasher {
  size_t operator()(const T &t) const {
    size_t hash;
    MurmurHash3_x86_32(&t, sizeof(t), 0, &hash);
    return hash;
  }
};

template <> struct MurmurHasher<OID *> {
  size_t operator()(const OID *t) const {
    size_t hash;
    MurmurHash3_x86_32((void *)t, sizeof(OID), 0, &hash);
    return hash;
  }
};
template <> struct MurmurHasher<NFKey *> {
  size_t operator()(const NFKey *t) const {
    size_t hash;
    MurmurHash3_x86_32((void *)t, sizeof(NFKey), 0, &hash);
    return hash;
  }
};

struct eqoidptr {
  bool operator()(const OID *s1, const OID *s2) const {
    return (s1->hpid == s2->hpid && s1->createTS == s2->createTS);
  }
};
struct eqoid {
  bool operator()(const OID &s1, const OID &s2) const {
    return (s1.hpid == s2.hpid && s1.createTS == s2.createTS);
  }
};

// specialization for strings
template <> struct MurmurHasher<string> {
  size_t operator()(const string &t) const {
    size_t hash;
    MurmurHash3_x86_32(t.c_str(), t.size(), 0, &hash);
    return hash;
  }
};

struct eqstr {
  bool operator()(const string &s1, const string &s2) const {
    return (s1.compare(s2) == 0);
  }
};

template <> struct MurmurHasher<NFKey> {
  size_t operator()(const NFKey &t) const {
    size_t hash = 0;
    MurmurHash3_x86_32((void *)&t, sizeof(NFKey), 0, &hash);
    return hash;
  }
};

struct eqnfkey {
  bool operator()(const NFKey &n1, const NFKey &n2) const {
    return (n1.ip1 == n2.ip1 && n1.ip2 == n2.ip2 && n1.port1 == n2.port1 &&
            n1.port2 == n2.port2 && n1.tid == n2.tid && n1.fd == n2.fd);
  }
};

struct eqdfobj {
  bool operator()(const DataFlowObj *df1, const DataFlowObj *df2) const {
    return (df1->exportTime < df2->exportTime);
  }
};


class FileObj {
public:
  bool written{false};
  uint32_t refs{0};
  string key;
  sysflow::File file;
  FileObj() {}
};

class ContainerObj {
public:
  bool written{false};
  bool incomplete{false};
  uint32_t refs{0};
  Container cont;
  ContainerObj() {}
};

typedef google::dense_hash_map<int, string> ParameterMapping;
typedef google::dense_hash_map<string, ContainerObj *, MurmurHasher<string>,
                               eqstr>
    ContainerTable;
typedef google::dense_hash_map<NFKey, NetFlowObj *, MurmurHasher<NFKey>,
                               eqnfkey>
    NetworkFlowTable;
typedef google::dense_hash_map<string, FileFlowObj *, MurmurHasher<string>,
                               eqstr>
    FileFlowTable;
typedef google::dense_hash_map<string, FileObj *, MurmurHasher<string>, eqstr>
    FileTable;
typedef google::dense_hash_map<OID, NetworkFlowTable *, MurmurHasher<OID>,
                               eqoid>
    OIDNetworkTable;
typedef google::dense_hash_set<OID, MurmurHasher<OID>, eqoid> ProcessSet;
typedef multiset<DataFlowObj *, eqdfobj> DataFlowSet;
typedef std::list<OIDObj *> OIDQueue;
class ProcessObj {
public:
  bool written{false};
  Process proc;
  NetworkFlowTable netflows;
  FileFlowTable fileflows;
  ProcessSet children;
  ProcessFlowObj* pfo;
  ProcessObj() : proc(), netflows(), fileflows(), children(), pfo(nullptr) {
    NFKey *emptykey = utils::getNFEmptyKey();
    NFKey *delkey = utils::getNFDelKey();
    OID *emptyoidkey = utils::getOIDEmptyKey();
    OID *deloidkey = utils::getOIDDelKey();
    netflows.set_empty_key(*emptykey);
    netflows.set_deleted_key(*delkey);
    fileflows.set_empty_key("-2");
    fileflows.set_deleted_key("-1");
    children.set_empty_key(*emptyoidkey);
    children.set_deleted_key(*deloidkey);
  }
  bool operator==(const ProcessObj &p) {
    if (pfo != nullptr && p.pfo != nullptr && pfo->exportTime != p.pfo->exportTime) {
      return false;
    }
    return (proc.oid.createTS == p.proc.oid.createTS && proc.oid.hpid == p.proc.oid.hpid);
  }
};

struct eqpfobj {
  bool operator()(const ProcessObj *p1, const ProcessObj *p2) const {
    return (p1->pfo->exportTime < p2->pfo->exportTime);
  }
};
typedef multiset<ProcessObj *, eqpfobj> ProcessFlowSet;
typedef google::dense_hash_map<OID *, ProcessObj *, MurmurHasher<OID *>,
                               eqoidptr>
    ProcessTable;

#endif

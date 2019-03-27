/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef __SRC_SYSFLOW_SYSFLOW_HH_810194984__H_
#define __SRC_SYSFLOW_SYSFLOW_HH_810194984__H_


#include <sstream>
#include "boost/any.hpp"
#include "avro/Specific.hh"
#include "avro/Encoder.hh"
#include "avro/Decoder.hh"

namespace sysflow {
struct SFHeader {
    int64_t version;
    std::string exporter;
    SFHeader() :
        version(int64_t()),
        exporter(std::string())
        { }
};

enum ContainerType {
    CT_DOCKER,
    CT_LXC,
    CT_LIBVIRT_LXC,
    CT_MESOS,
    CT_RKT,
    CT_CUSTOM,
};

struct Container {
    std::string id;
    std::string name;
    std::string image;
    std::string imageid;
    ContainerType type;
    Container() :
        id(std::string()),
        name(std::string()),
        image(std::string()),
        imageid(std::string()),
        type(ContainerType())
        { }
};

enum ActionType {
    CREATED,
    MODIFIED,
    REUP,
};

struct OID {
    int64_t createTS;
    int64_t hpid;
    OID() :
        createTS(int64_t()),
        hpid(int64_t())
        { }
};

struct _SysFlow_avsc_Union__0__ {
private:
    size_t idx_;
    boost::any value_;
public:
    size_t idx() const { return idx_; }
    bool is_null() const {
        return (idx_ == 0);
    }
    void set_null() {
        idx_ = 0;
        value_ = boost::any();
    }
    OID get_OID() const;
    void set_OID(const OID& v);
    _SysFlow_avsc_Union__0__();
};

struct _SysFlow_avsc_Union__1__ {
private:
    size_t idx_;
    boost::any value_;
public:
    size_t idx() const { return idx_; }
    bool is_null() const {
        return (idx_ == 0);
    }
    void set_null() {
        idx_ = 0;
        value_ = boost::any();
    }
    std::string get_string() const;
    void set_string(const std::string& v);
    _SysFlow_avsc_Union__1__();
};

struct Process {
    typedef _SysFlow_avsc_Union__0__ poid_t;
    typedef _SysFlow_avsc_Union__1__ containerId_t;
    ActionType type;
    OID oid;
    poid_t poid;
    int64_t ts;
    std::string exe;
    std::string exeArgs;
    int32_t uid;
    std::string userName;
    int32_t gid;
    std::string groupName;
    containerId_t containerId;
    Process() :
        type(ActionType()),
        oid(OID()),
        poid(poid_t()),
        ts(int64_t()),
        exe(std::string()),
        exeArgs(std::string()),
        uid(int32_t()),
        userName(std::string()),
        gid(int32_t()),
        groupName(std::string()),
        containerId(containerId_t())
        { }
};

struct FOID {
    int64_t upperbits;
    int64_t lowerbits;
    FOID() :
        upperbits(int64_t()),
        lowerbits(int64_t())
        { }
};

enum ResourceType {
    FILE,
    DIRECTORY,
    PIPE,
    UNIX,
};

struct _SysFlow_avsc_Union__2__ {
private:
    size_t idx_;
    boost::any value_;
public:
    size_t idx() const { return idx_; }
    bool is_null() const {
        return (idx_ == 0);
    }
    void set_null() {
        idx_ = 0;
        value_ = boost::any();
    }
    std::string get_string() const;
    void set_string(const std::string& v);
    _SysFlow_avsc_Union__2__();
};

struct File {
    typedef _SysFlow_avsc_Union__2__ containerDirectory_t;
    ActionType type;
    FOID oid;
    int64_t ts;
    ResourceType restype;
    std::string path;
    containerDirectory_t containerDirectory;
    File() :
        type(ActionType()),
        oid(FOID()),
        ts(int64_t()),
        restype(ResourceType()),
        path(std::string()),
        containerDirectory(containerDirectory_t())
        { }
};

enum EventType {
    CLONE,
    EXEC,
    EXIT,
};

struct ProcessFlow {
    OID procOID;
    int64_t ts;
    int64_t tid;
    EventType type;
    std::vector<std::string > args;
    int32_t ret;
    ProcessFlow() :
        procOID(OID()),
        ts(int64_t()),
        tid(int64_t()),
        type(EventType()),
        args(std::vector<std::string >()),
        ret(int32_t())
        { }
};

struct NetworkFlow {
    OID procOID;
    int64_t ts;
    int64_t tid;
    int32_t opFlags;
    int64_t endTs;
    int32_t sip;
    int32_t sport;
    int32_t dip;
    int32_t dport;
    int32_t proto;
    int64_t numROps;
    int64_t numWOps;
    int64_t numRBytes;
    int64_t numWBytes;
    NetworkFlow() :
        procOID(OID()),
        ts(int64_t()),
        tid(int64_t()),
        opFlags(int32_t()),
        endTs(int64_t()),
        sip(int32_t()),
        sport(int32_t()),
        dip(int32_t()),
        dport(int32_t()),
        proto(int32_t()),
        numROps(int64_t()),
        numWOps(int64_t()),
        numRBytes(int64_t()),
        numWBytes(int64_t())
        { }
};

struct FileFlow {
    OID procOID;
    int64_t ts;
    int64_t tid;
    int32_t opFlags;
    int64_t endTs;
    FOID fileOID;
    int32_t fd;
    int64_t numROps;
    int64_t numWOps;
    int64_t numRBytes;
    int64_t numWBytes;
    FileFlow() :
        procOID(OID()),
        ts(int64_t()),
        tid(int64_t()),
        opFlags(int32_t()),
        endTs(int64_t()),
        fileOID(FOID()),
        fd(int32_t()),
        numROps(int64_t()),
        numWOps(int64_t()),
        numRBytes(int64_t()),
        numWBytes(int64_t())
        { }
};

struct _SysFlow_avsc_Union__3__ {
private:
    size_t idx_;
    boost::any value_;
public:
    size_t idx() const { return idx_; }
    SFHeader get_SFHeader() const;
    void set_SFHeader(const SFHeader& v);
    Container get_Container() const;
    void set_Container(const Container& v);
    Process get_Process() const;
    void set_Process(const Process& v);
    File get_File() const;
    void set_File(const File& v);
    ProcessFlow get_ProcessFlow() const;
    void set_ProcessFlow(const ProcessFlow& v);
    NetworkFlow get_NetworkFlow() const;
    void set_NetworkFlow(const NetworkFlow& v);
    FileFlow get_FileFlow() const;
    void set_FileFlow(const FileFlow& v);
    _SysFlow_avsc_Union__3__();
};

struct SysFlow {
    typedef _SysFlow_avsc_Union__3__ rec_t;
    rec_t rec;
    SysFlow() :
        rec(rec_t())
        { }
};

inline
OID _SysFlow_avsc_Union__0__::get_OID() const {
    if (idx_ != 1) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<OID >(value_);
}

inline
void _SysFlow_avsc_Union__0__::set_OID(const OID& v) {
    idx_ = 1;
    value_ = v;
}

inline
std::string _SysFlow_avsc_Union__1__::get_string() const {
    if (idx_ != 1) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<std::string >(value_);
}

inline
void _SysFlow_avsc_Union__1__::set_string(const std::string& v) {
    idx_ = 1;
    value_ = v;
}

inline
std::string _SysFlow_avsc_Union__2__::get_string() const {
    if (idx_ != 1) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<std::string >(value_);
}

inline
void _SysFlow_avsc_Union__2__::set_string(const std::string& v) {
    idx_ = 1;
    value_ = v;
}

inline
SFHeader _SysFlow_avsc_Union__3__::get_SFHeader() const {
    if (idx_ != 0) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<SFHeader >(value_);
}

inline
void _SysFlow_avsc_Union__3__::set_SFHeader(const SFHeader& v) {
    idx_ = 0;
    value_ = v;
}

inline
Container _SysFlow_avsc_Union__3__::get_Container() const {
    if (idx_ != 1) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<Container >(value_);
}

inline
void _SysFlow_avsc_Union__3__::set_Container(const Container& v) {
    idx_ = 1;
    value_ = v;
}

inline
Process _SysFlow_avsc_Union__3__::get_Process() const {
    if (idx_ != 2) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<Process >(value_);
}

inline
void _SysFlow_avsc_Union__3__::set_Process(const Process& v) {
    idx_ = 2;
    value_ = v;
}

inline
File _SysFlow_avsc_Union__3__::get_File() const {
    if (idx_ != 3) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<File >(value_);
}

inline
void _SysFlow_avsc_Union__3__::set_File(const File& v) {
    idx_ = 3;
    value_ = v;
}

inline
ProcessFlow _SysFlow_avsc_Union__3__::get_ProcessFlow() const {
    if (idx_ != 4) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<ProcessFlow >(value_);
}

inline
void _SysFlow_avsc_Union__3__::set_ProcessFlow(const ProcessFlow& v) {
    idx_ = 4;
    value_ = v;
}

inline
NetworkFlow _SysFlow_avsc_Union__3__::get_NetworkFlow() const {
    if (idx_ != 5) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<NetworkFlow >(value_);
}

inline
void _SysFlow_avsc_Union__3__::set_NetworkFlow(const NetworkFlow& v) {
    idx_ = 5;
    value_ = v;
}

inline
FileFlow _SysFlow_avsc_Union__3__::get_FileFlow() const {
    if (idx_ != 6) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<FileFlow >(value_);
}

inline
void _SysFlow_avsc_Union__3__::set_FileFlow(const FileFlow& v) {
    idx_ = 6;
    value_ = v;
}

inline _SysFlow_avsc_Union__0__::_SysFlow_avsc_Union__0__() : idx_(0) { }
inline _SysFlow_avsc_Union__1__::_SysFlow_avsc_Union__1__() : idx_(0) { }
inline _SysFlow_avsc_Union__2__::_SysFlow_avsc_Union__2__() : idx_(0) { }
inline _SysFlow_avsc_Union__3__::_SysFlow_avsc_Union__3__() : idx_(0), value_(SFHeader()) { }
}
namespace avro {
template<> struct codec_traits<sysflow::SFHeader> {
    static void encode(Encoder& e, const sysflow::SFHeader& v) {
        avro::encode(e, v.version);
        avro::encode(e, v.exporter);
    }
    static void decode(Decoder& d, sysflow::SFHeader& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.version);
                    break;
                case 1:
                    avro::decode(d, v.exporter);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.version);
            avro::decode(d, v.exporter);
        }
    }
};

template<> struct codec_traits<sysflow::ContainerType> {
    static void encode(Encoder& e, sysflow::ContainerType v) {
		if (v < sysflow::CT_DOCKER || v > sysflow::CT_CUSTOM)
		{
			std::ostringstream error;
			error << "enum value " << v << " is out of bound for sysflow::ContainerType and cannot be encoded";
			throw avro::Exception(error.str());
		}
        e.encodeEnum(v);
    }
    static void decode(Decoder& d, sysflow::ContainerType& v) {
		size_t index = d.decodeEnum();
		if (index < sysflow::CT_DOCKER || index > sysflow::CT_CUSTOM)
		{
			std::ostringstream error;
			error << "enum value " << index << " is out of bound for sysflow::ContainerType and cannot be decoded";
			throw avro::Exception(error.str());
		}
        v = static_cast<sysflow::ContainerType>(index);
    }
};

template<> struct codec_traits<sysflow::Container> {
    static void encode(Encoder& e, const sysflow::Container& v) {
        avro::encode(e, v.id);
        avro::encode(e, v.name);
        avro::encode(e, v.image);
        avro::encode(e, v.imageid);
        avro::encode(e, v.type);
    }
    static void decode(Decoder& d, sysflow::Container& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.id);
                    break;
                case 1:
                    avro::decode(d, v.name);
                    break;
                case 2:
                    avro::decode(d, v.image);
                    break;
                case 3:
                    avro::decode(d, v.imageid);
                    break;
                case 4:
                    avro::decode(d, v.type);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.id);
            avro::decode(d, v.name);
            avro::decode(d, v.image);
            avro::decode(d, v.imageid);
            avro::decode(d, v.type);
        }
    }
};

template<> struct codec_traits<sysflow::ActionType> {
    static void encode(Encoder& e, sysflow::ActionType v) {
		if (v < sysflow::CREATED || v > sysflow::REUP)
		{
			std::ostringstream error;
			error << "enum value " << v << " is out of bound for sysflow::ActionType and cannot be encoded";
			throw avro::Exception(error.str());
		}
        e.encodeEnum(v);
    }
    static void decode(Decoder& d, sysflow::ActionType& v) {
		size_t index = d.decodeEnum();
		if (index < sysflow::CREATED || index > sysflow::REUP)
		{
			std::ostringstream error;
			error << "enum value " << index << " is out of bound for sysflow::ActionType and cannot be decoded";
			throw avro::Exception(error.str());
		}
        v = static_cast<sysflow::ActionType>(index);
    }
};

template<> struct codec_traits<sysflow::OID> {
    static void encode(Encoder& e, const sysflow::OID& v) {
        avro::encode(e, v.createTS);
        avro::encode(e, v.hpid);
    }
    static void decode(Decoder& d, sysflow::OID& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.createTS);
                    break;
                case 1:
                    avro::decode(d, v.hpid);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.createTS);
            avro::decode(d, v.hpid);
        }
    }
};

template<> struct codec_traits<sysflow::_SysFlow_avsc_Union__0__> {
    static void encode(Encoder& e, sysflow::_SysFlow_avsc_Union__0__ v) {
        e.encodeUnionIndex(v.idx());
        switch (v.idx()) {
        case 0:
            e.encodeNull();
            break;
        case 1:
            avro::encode(e, v.get_OID());
            break;
        }
    }
    static void decode(Decoder& d, sysflow::_SysFlow_avsc_Union__0__& v) {
        size_t n = d.decodeUnionIndex();
        if (n >= 2) { throw avro::Exception("Union index too big"); }
        switch (n) {
        case 0:
            d.decodeNull();
            v.set_null();
            break;
        case 1:
            {
                sysflow::OID vv;
                avro::decode(d, vv);
                v.set_OID(vv);
            }
            break;
        }
    }
};

template<> struct codec_traits<sysflow::_SysFlow_avsc_Union__1__> {
    static void encode(Encoder& e, sysflow::_SysFlow_avsc_Union__1__ v) {
        e.encodeUnionIndex(v.idx());
        switch (v.idx()) {
        case 0:
            e.encodeNull();
            break;
        case 1:
            avro::encode(e, v.get_string());
            break;
        }
    }
    static void decode(Decoder& d, sysflow::_SysFlow_avsc_Union__1__& v) {
        size_t n = d.decodeUnionIndex();
        if (n >= 2) { throw avro::Exception("Union index too big"); }
        switch (n) {
        case 0:
            d.decodeNull();
            v.set_null();
            break;
        case 1:
            {
                std::string vv;
                avro::decode(d, vv);
                v.set_string(vv);
            }
            break;
        }
    }
};

template<> struct codec_traits<sysflow::Process> {
    static void encode(Encoder& e, const sysflow::Process& v) {
        avro::encode(e, v.type);
        avro::encode(e, v.oid);
        avro::encode(e, v.poid);
        avro::encode(e, v.ts);
        avro::encode(e, v.exe);
        avro::encode(e, v.exeArgs);
        avro::encode(e, v.uid);
        avro::encode(e, v.userName);
        avro::encode(e, v.gid);
        avro::encode(e, v.groupName);
        avro::encode(e, v.containerId);
    }
    static void decode(Decoder& d, sysflow::Process& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.type);
                    break;
                case 1:
                    avro::decode(d, v.oid);
                    break;
                case 2:
                    avro::decode(d, v.poid);
                    break;
                case 3:
                    avro::decode(d, v.ts);
                    break;
                case 4:
                    avro::decode(d, v.exe);
                    break;
                case 5:
                    avro::decode(d, v.exeArgs);
                    break;
                case 6:
                    avro::decode(d, v.uid);
                    break;
                case 7:
                    avro::decode(d, v.userName);
                    break;
                case 8:
                    avro::decode(d, v.gid);
                    break;
                case 9:
                    avro::decode(d, v.groupName);
                    break;
                case 10:
                    avro::decode(d, v.containerId);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.type);
            avro::decode(d, v.oid);
            avro::decode(d, v.poid);
            avro::decode(d, v.ts);
            avro::decode(d, v.exe);
            avro::decode(d, v.exeArgs);
            avro::decode(d, v.uid);
            avro::decode(d, v.userName);
            avro::decode(d, v.gid);
            avro::decode(d, v.groupName);
            avro::decode(d, v.containerId);
        }
    }
};

template<> struct codec_traits<sysflow::FOID> {
    static void encode(Encoder& e, const sysflow::FOID& v) {
        avro::encode(e, v.upperbits);
        avro::encode(e, v.lowerbits);
    }
    static void decode(Decoder& d, sysflow::FOID& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.upperbits);
                    break;
                case 1:
                    avro::decode(d, v.lowerbits);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.upperbits);
            avro::decode(d, v.lowerbits);
        }
    }
};

template<> struct codec_traits<sysflow::ResourceType> {
    static void encode(Encoder& e, sysflow::ResourceType v) {
		if (v < sysflow::FILE || v > sysflow::UNIX)
		{
			std::ostringstream error;
			error << "enum value " << v << " is out of bound for sysflow::ResourceType and cannot be encoded";
			throw avro::Exception(error.str());
		}
        e.encodeEnum(v);
    }
    static void decode(Decoder& d, sysflow::ResourceType& v) {
		size_t index = d.decodeEnum();
		if (index < sysflow::FILE || index > sysflow::UNIX)
		{
			std::ostringstream error;
			error << "enum value " << index << " is out of bound for sysflow::ResourceType and cannot be decoded";
			throw avro::Exception(error.str());
		}
        v = static_cast<sysflow::ResourceType>(index);
    }
};

template<> struct codec_traits<sysflow::_SysFlow_avsc_Union__2__> {
    static void encode(Encoder& e, sysflow::_SysFlow_avsc_Union__2__ v) {
        e.encodeUnionIndex(v.idx());
        switch (v.idx()) {
        case 0:
            e.encodeNull();
            break;
        case 1:
            avro::encode(e, v.get_string());
            break;
        }
    }
    static void decode(Decoder& d, sysflow::_SysFlow_avsc_Union__2__& v) {
        size_t n = d.decodeUnionIndex();
        if (n >= 2) { throw avro::Exception("Union index too big"); }
        switch (n) {
        case 0:
            d.decodeNull();
            v.set_null();
            break;
        case 1:
            {
                std::string vv;
                avro::decode(d, vv);
                v.set_string(vv);
            }
            break;
        }
    }
};

template<> struct codec_traits<sysflow::File> {
    static void encode(Encoder& e, const sysflow::File& v) {
        avro::encode(e, v.type);
        avro::encode(e, v.oid);
        avro::encode(e, v.ts);
        avro::encode(e, v.restype);
        avro::encode(e, v.path);
        avro::encode(e, v.containerDirectory);
    }
    static void decode(Decoder& d, sysflow::File& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.type);
                    break;
                case 1:
                    avro::decode(d, v.oid);
                    break;
                case 2:
                    avro::decode(d, v.ts);
                    break;
                case 3:
                    avro::decode(d, v.restype);
                    break;
                case 4:
                    avro::decode(d, v.path);
                    break;
                case 5:
                    avro::decode(d, v.containerDirectory);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.type);
            avro::decode(d, v.oid);
            avro::decode(d, v.ts);
            avro::decode(d, v.restype);
            avro::decode(d, v.path);
            avro::decode(d, v.containerDirectory);
        }
    }
};

template<> struct codec_traits<sysflow::EventType> {
    static void encode(Encoder& e, sysflow::EventType v) {
		if (v < sysflow::CLONE || v > sysflow::EXIT)
		{
			std::ostringstream error;
			error << "enum value " << v << " is out of bound for sysflow::EventType and cannot be encoded";
			throw avro::Exception(error.str());
		}
        e.encodeEnum(v);
    }
    static void decode(Decoder& d, sysflow::EventType& v) {
		size_t index = d.decodeEnum();
		if (index < sysflow::CLONE || index > sysflow::EXIT)
		{
			std::ostringstream error;
			error << "enum value " << index << " is out of bound for sysflow::EventType and cannot be decoded";
			throw avro::Exception(error.str());
		}
        v = static_cast<sysflow::EventType>(index);
    }
};

template<> struct codec_traits<sysflow::ProcessFlow> {
    static void encode(Encoder& e, const sysflow::ProcessFlow& v) {
        avro::encode(e, v.procOID);
        avro::encode(e, v.ts);
        avro::encode(e, v.tid);
        avro::encode(e, v.type);
        avro::encode(e, v.args);
        avro::encode(e, v.ret);
    }
    static void decode(Decoder& d, sysflow::ProcessFlow& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.procOID);
                    break;
                case 1:
                    avro::decode(d, v.ts);
                    break;
                case 2:
                    avro::decode(d, v.tid);
                    break;
                case 3:
                    avro::decode(d, v.type);
                    break;
                case 4:
                    avro::decode(d, v.args);
                    break;
                case 5:
                    avro::decode(d, v.ret);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.procOID);
            avro::decode(d, v.ts);
            avro::decode(d, v.tid);
            avro::decode(d, v.type);
            avro::decode(d, v.args);
            avro::decode(d, v.ret);
        }
    }
};

template<> struct codec_traits<sysflow::NetworkFlow> {
    static void encode(Encoder& e, const sysflow::NetworkFlow& v) {
        avro::encode(e, v.procOID);
        avro::encode(e, v.ts);
        avro::encode(e, v.tid);
        avro::encode(e, v.opFlags);
        avro::encode(e, v.endTs);
        avro::encode(e, v.sip);
        avro::encode(e, v.sport);
        avro::encode(e, v.dip);
        avro::encode(e, v.dport);
        avro::encode(e, v.proto);
        avro::encode(e, v.numROps);
        avro::encode(e, v.numWOps);
        avro::encode(e, v.numRBytes);
        avro::encode(e, v.numWBytes);
    }
    static void decode(Decoder& d, sysflow::NetworkFlow& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.procOID);
                    break;
                case 1:
                    avro::decode(d, v.ts);
                    break;
                case 2:
                    avro::decode(d, v.tid);
                    break;
                case 3:
                    avro::decode(d, v.opFlags);
                    break;
                case 4:
                    avro::decode(d, v.endTs);
                    break;
                case 5:
                    avro::decode(d, v.sip);
                    break;
                case 6:
                    avro::decode(d, v.sport);
                    break;
                case 7:
                    avro::decode(d, v.dip);
                    break;
                case 8:
                    avro::decode(d, v.dport);
                    break;
                case 9:
                    avro::decode(d, v.proto);
                    break;
                case 10:
                    avro::decode(d, v.numROps);
                    break;
                case 11:
                    avro::decode(d, v.numWOps);
                    break;
                case 12:
                    avro::decode(d, v.numRBytes);
                    break;
                case 13:
                    avro::decode(d, v.numWBytes);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.procOID);
            avro::decode(d, v.ts);
            avro::decode(d, v.tid);
            avro::decode(d, v.opFlags);
            avro::decode(d, v.endTs);
            avro::decode(d, v.sip);
            avro::decode(d, v.sport);
            avro::decode(d, v.dip);
            avro::decode(d, v.dport);
            avro::decode(d, v.proto);
            avro::decode(d, v.numROps);
            avro::decode(d, v.numWOps);
            avro::decode(d, v.numRBytes);
            avro::decode(d, v.numWBytes);
        }
    }
};

template<> struct codec_traits<sysflow::FileFlow> {
    static void encode(Encoder& e, const sysflow::FileFlow& v) {
        avro::encode(e, v.procOID);
        avro::encode(e, v.ts);
        avro::encode(e, v.tid);
        avro::encode(e, v.opFlags);
        avro::encode(e, v.endTs);
        avro::encode(e, v.fileOID);
        avro::encode(e, v.fd);
        avro::encode(e, v.numROps);
        avro::encode(e, v.numWOps);
        avro::encode(e, v.numRBytes);
        avro::encode(e, v.numWBytes);
    }
    static void decode(Decoder& d, sysflow::FileFlow& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.procOID);
                    break;
                case 1:
                    avro::decode(d, v.ts);
                    break;
                case 2:
                    avro::decode(d, v.tid);
                    break;
                case 3:
                    avro::decode(d, v.opFlags);
                    break;
                case 4:
                    avro::decode(d, v.endTs);
                    break;
                case 5:
                    avro::decode(d, v.fileOID);
                    break;
                case 6:
                    avro::decode(d, v.fd);
                    break;
                case 7:
                    avro::decode(d, v.numROps);
                    break;
                case 8:
                    avro::decode(d, v.numWOps);
                    break;
                case 9:
                    avro::decode(d, v.numRBytes);
                    break;
                case 10:
                    avro::decode(d, v.numWBytes);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.procOID);
            avro::decode(d, v.ts);
            avro::decode(d, v.tid);
            avro::decode(d, v.opFlags);
            avro::decode(d, v.endTs);
            avro::decode(d, v.fileOID);
            avro::decode(d, v.fd);
            avro::decode(d, v.numROps);
            avro::decode(d, v.numWOps);
            avro::decode(d, v.numRBytes);
            avro::decode(d, v.numWBytes);
        }
    }
};

template<> struct codec_traits<sysflow::_SysFlow_avsc_Union__3__> {
    static void encode(Encoder& e, sysflow::_SysFlow_avsc_Union__3__ v) {
        e.encodeUnionIndex(v.idx());
        switch (v.idx()) {
        case 0:
            avro::encode(e, v.get_SFHeader());
            break;
        case 1:
            avro::encode(e, v.get_Container());
            break;
        case 2:
            avro::encode(e, v.get_Process());
            break;
        case 3:
            avro::encode(e, v.get_File());
            break;
        case 4:
            avro::encode(e, v.get_ProcessFlow());
            break;
        case 5:
            avro::encode(e, v.get_NetworkFlow());
            break;
        case 6:
            avro::encode(e, v.get_FileFlow());
            break;
        }
    }
    static void decode(Decoder& d, sysflow::_SysFlow_avsc_Union__3__& v) {
        size_t n = d.decodeUnionIndex();
        if (n >= 7) { throw avro::Exception("Union index too big"); }
        switch (n) {
        case 0:
            {
                sysflow::SFHeader vv;
                avro::decode(d, vv);
                v.set_SFHeader(vv);
            }
            break;
        case 1:
            {
                sysflow::Container vv;
                avro::decode(d, vv);
                v.set_Container(vv);
            }
            break;
        case 2:
            {
                sysflow::Process vv;
                avro::decode(d, vv);
                v.set_Process(vv);
            }
            break;
        case 3:
            {
                sysflow::File vv;
                avro::decode(d, vv);
                v.set_File(vv);
            }
            break;
        case 4:
            {
                sysflow::ProcessFlow vv;
                avro::decode(d, vv);
                v.set_ProcessFlow(vv);
            }
            break;
        case 5:
            {
                sysflow::NetworkFlow vv;
                avro::decode(d, vv);
                v.set_NetworkFlow(vv);
            }
            break;
        case 6:
            {
                sysflow::FileFlow vv;
                avro::decode(d, vv);
                v.set_FileFlow(vv);
            }
            break;
        }
    }
};

template<> struct codec_traits<sysflow::SysFlow> {
    static void encode(Encoder& e, const sysflow::SysFlow& v) {
        avro::encode(e, v.rec);
    }
    static void decode(Decoder& d, sysflow::SysFlow& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.rec);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.rec);
        }
    }
};

}
#endif

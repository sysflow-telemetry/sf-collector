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


#ifndef __SRC_SYSFLOW_PROCESS_HH_810194984__H_
#define __SRC_SYSFLOW_PROCESS_HH_810194984__H_


#include <sstream>
#include "boost/any.hpp"
#include "avro/Specific.hh"
#include "avro/Encoder.hh"
#include "avro/Decoder.hh"

namespace sysflow.entity {
enum SFObjectState {
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

struct _Process_avsc_Union__0__ {
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
    _Process_avsc_Union__0__();
};

struct _Process_avsc_Union__1__ {
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
    _Process_avsc_Union__1__();
};

struct Process {
    typedef _Process_avsc_Union__0__ poid_t;
    typedef _Process_avsc_Union__1__ containerId_t;
    SFObjectState state;
    OID oid;
    poid_t poid;
    int64_t ts;
    std::string exe;
    std::string exeArgs;
    int32_t uid;
    std::string userName;
    int32_t gid;
    std::string groupName;
    bool tty;
    containerId_t containerId;
    Process() :
        state(SFObjectState()),
        oid(OID()),
        poid(poid_t()),
        ts(int64_t()),
        exe(std::string()),
        exeArgs(std::string()),
        uid(int32_t()),
        userName(std::string()),
        gid(int32_t()),
        groupName(std::string()),
        tty(bool()),
        containerId(containerId_t())
        { }
};

inline
OID _Process_avsc_Union__0__::get_OID() const {
    if (idx_ != 1) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<OID >(value_);
}

inline
void _Process_avsc_Union__0__::set_OID(const OID& v) {
    idx_ = 1;
    value_ = v;
}

inline
std::string _Process_avsc_Union__1__::get_string() const {
    if (idx_ != 1) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<std::string >(value_);
}

inline
void _Process_avsc_Union__1__::set_string(const std::string& v) {
    idx_ = 1;
    value_ = v;
}

inline _Process_avsc_Union__0__::_Process_avsc_Union__0__() : idx_(0) { }
inline _Process_avsc_Union__1__::_Process_avsc_Union__1__() : idx_(0) { }
}
namespace avro {
template<> struct codec_traits<sysflow.entity::SFObjectState> {
    static void encode(Encoder& e, sysflow.entity::SFObjectState v) {
		if (v < sysflow.entity::CREATED || v > sysflow.entity::REUP)
		{
			std::ostringstream error;
			error << "enum value " << v << " is out of bound for sysflow.entity::SFObjectState and cannot be encoded";
			throw avro::Exception(error.str());
		}
        e.encodeEnum(v);
    }
    static void decode(Decoder& d, sysflow.entity::SFObjectState& v) {
		size_t index = d.decodeEnum();
		if (index < sysflow.entity::CREATED || index > sysflow.entity::REUP)
		{
			std::ostringstream error;
			error << "enum value " << index << " is out of bound for sysflow.entity::SFObjectState and cannot be decoded";
			throw avro::Exception(error.str());
		}
        v = static_cast<sysflow.entity::SFObjectState>(index);
    }
};

template<> struct codec_traits<sysflow.entity::OID> {
    static void encode(Encoder& e, const sysflow.entity::OID& v) {
        avro::encode(e, v.createTS);
        avro::encode(e, v.hpid);
    }
    static void decode(Decoder& d, sysflow.entity::OID& v) {
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

template<> struct codec_traits<sysflow.entity::_Process_avsc_Union__0__> {
    static void encode(Encoder& e, sysflow.entity::_Process_avsc_Union__0__ v) {
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
    static void decode(Decoder& d, sysflow.entity::_Process_avsc_Union__0__& v) {
        size_t n = d.decodeUnionIndex();
        if (n >= 2) { throw avro::Exception("Union index too big"); }
        switch (n) {
        case 0:
            d.decodeNull();
            v.set_null();
            break;
        case 1:
            {
                sysflow.entity::OID vv;
                avro::decode(d, vv);
                v.set_OID(vv);
            }
            break;
        }
    }
};

template<> struct codec_traits<sysflow.entity::_Process_avsc_Union__1__> {
    static void encode(Encoder& e, sysflow.entity::_Process_avsc_Union__1__ v) {
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
    static void decode(Decoder& d, sysflow.entity::_Process_avsc_Union__1__& v) {
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

template<> struct codec_traits<sysflow.entity::Process> {
    static void encode(Encoder& e, const sysflow.entity::Process& v) {
        avro::encode(e, v.state);
        avro::encode(e, v.oid);
        avro::encode(e, v.poid);
        avro::encode(e, v.ts);
        avro::encode(e, v.exe);
        avro::encode(e, v.exeArgs);
        avro::encode(e, v.uid);
        avro::encode(e, v.userName);
        avro::encode(e, v.gid);
        avro::encode(e, v.groupName);
        avro::encode(e, v.tty);
        avro::encode(e, v.containerId);
    }
    static void decode(Decoder& d, sysflow.entity::Process& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.state);
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
                    avro::decode(d, v.tty);
                    break;
                case 11:
                    avro::decode(d, v.containerId);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.state);
            avro::decode(d, v.oid);
            avro::decode(d, v.poid);
            avro::decode(d, v.ts);
            avro::decode(d, v.exe);
            avro::decode(d, v.exeArgs);
            avro::decode(d, v.uid);
            avro::decode(d, v.userName);
            avro::decode(d, v.gid);
            avro::decode(d, v.groupName);
            avro::decode(d, v.tty);
            avro::decode(d, v.containerId);
        }
    }
};

}
#endif

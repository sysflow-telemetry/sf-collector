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


#ifndef __SRC_SYSFLOW_FILE_HH_4165041343__H_
#define __SRC_SYSFLOW_FILE_HH_4165041343__H_


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

struct _File_avsc_Union__0__ {
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
    _File_avsc_Union__0__();
};

struct File {
    typedef _File_avsc_Union__0__ containerId_t;
    SFObjectState state;
    boost::array<uint8_t, 20> oid;
    int64_t ts;
    int32_t restype;
    std::string path;
    containerId_t containerId;
    File() :
        state(SFObjectState()),
        oid(boost::array<uint8_t, 20>()),
        ts(int64_t()),
        restype(int32_t()),
        path(std::string()),
        containerId(containerId_t())
        { }
};

inline
std::string _File_avsc_Union__0__::get_string() const {
    if (idx_ != 1) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<std::string >(value_);
}

inline
void _File_avsc_Union__0__::set_string(const std::string& v) {
    idx_ = 1;
    value_ = v;
}

inline _File_avsc_Union__0__::_File_avsc_Union__0__() : idx_(0) { }
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

template<> struct codec_traits<sysflow.entity::_File_avsc_Union__0__> {
    static void encode(Encoder& e, sysflow.entity::_File_avsc_Union__0__ v) {
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
    static void decode(Decoder& d, sysflow.entity::_File_avsc_Union__0__& v) {
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

template<> struct codec_traits<sysflow.entity::File> {
    static void encode(Encoder& e, const sysflow.entity::File& v) {
        avro::encode(e, v.state);
        avro::encode(e, v.oid);
        avro::encode(e, v.ts);
        avro::encode(e, v.restype);
        avro::encode(e, v.path);
        avro::encode(e, v.containerId);
    }
    static void decode(Decoder& d, sysflow.entity::File& v) {
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
                    avro::decode(d, v.ts);
                    break;
                case 3:
                    avro::decode(d, v.restype);
                    break;
                case 4:
                    avro::decode(d, v.path);
                    break;
                case 5:
                    avro::decode(d, v.containerId);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.state);
            avro::decode(d, v.oid);
            avro::decode(d, v.ts);
            avro::decode(d, v.restype);
            avro::decode(d, v.path);
            avro::decode(d, v.containerId);
        }
    }
};

}
#endif

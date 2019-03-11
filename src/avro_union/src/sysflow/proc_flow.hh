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


#ifndef __SRC_SYSFLOW_PROC_FLOW_HH_810194984__H_
#define __SRC_SYSFLOW_PROC_FLOW_HH_810194984__H_


#include <sstream>
#include "boost/any.hpp"
#include "avro/Specific.hh"
#include "avro/Encoder.hh"
#include "avro/Decoder.hh"

namespace sysflow.flow {
enum EventType {
    CLONE,
    EXEC,
    EXIT,
};

struct OID {
    int64_t createTS;
    int64_t hpid;
    OID() :
        createTS(int64_t()),
        hpid(int64_t())
        { }
};

struct ProcessFlow {
    EventType type;
    int64_t ts;
    OID procOID;
    int64_t tid;
    std::vector<std::string > args;
    int32_t ret;
    ProcessFlow() :
        type(EventType()),
        ts(int64_t()),
        procOID(OID()),
        tid(int64_t()),
        args(std::vector<std::string >()),
        ret(int32_t())
        { }
};

}
namespace avro {
template<> struct codec_traits<sysflow.flow::EventType> {
    static void encode(Encoder& e, sysflow.flow::EventType v) {
		if (v < sysflow.flow::CLONE || v > sysflow.flow::EXIT)
		{
			std::ostringstream error;
			error << "enum value " << v << " is out of bound for sysflow.flow::EventType and cannot be encoded";
			throw avro::Exception(error.str());
		}
        e.encodeEnum(v);
    }
    static void decode(Decoder& d, sysflow.flow::EventType& v) {
		size_t index = d.decodeEnum();
		if (index < sysflow.flow::CLONE || index > sysflow.flow::EXIT)
		{
			std::ostringstream error;
			error << "enum value " << index << " is out of bound for sysflow.flow::EventType and cannot be decoded";
			throw avro::Exception(error.str());
		}
        v = static_cast<sysflow.flow::EventType>(index);
    }
};

template<> struct codec_traits<sysflow.flow::OID> {
    static void encode(Encoder& e, const sysflow.flow::OID& v) {
        avro::encode(e, v.createTS);
        avro::encode(e, v.hpid);
    }
    static void decode(Decoder& d, sysflow.flow::OID& v) {
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

template<> struct codec_traits<sysflow.flow::ProcessFlow> {
    static void encode(Encoder& e, const sysflow.flow::ProcessFlow& v) {
        avro::encode(e, v.type);
        avro::encode(e, v.ts);
        avro::encode(e, v.procOID);
        avro::encode(e, v.tid);
        avro::encode(e, v.args);
        avro::encode(e, v.ret);
    }
    static void decode(Decoder& d, sysflow.flow::ProcessFlow& v) {
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
                    avro::decode(d, v.ts);
                    break;
                case 2:
                    avro::decode(d, v.procOID);
                    break;
                case 3:
                    avro::decode(d, v.tid);
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
            avro::decode(d, v.type);
            avro::decode(d, v.ts);
            avro::decode(d, v.procOID);
            avro::decode(d, v.tid);
            avro::decode(d, v.args);
            avro::decode(d, v.ret);
        }
    }
};

}
#endif

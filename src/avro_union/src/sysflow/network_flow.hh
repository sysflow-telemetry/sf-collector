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


#ifndef __SRC_SYSFLOW_NETWORK_FLOW_HH_3752645669__H_
#define __SRC_SYSFLOW_NETWORK_FLOW_HH_3752645669__H_


#include <sstream>
#include "boost/any.hpp"
#include "avro/Specific.hh"
#include "avro/Encoder.hh"
#include "avro/Decoder.hh"

namespace sysflow.flow {
struct OID {
    int64_t createTS;
    int64_t hpid;
    OID() :
        createTS(int64_t()),
        hpid(int64_t())
        { }
};

struct NetworkFlow {
    int32_t opFlags;
    int64_t startTs;
    int64_t endTs;
    OID procOID;
    int64_t tid;
    int32_t sip;
    int32_t sport;
    int32_t dip;
    int32_t dport;
    int32_t proto;
    int64_t numReads;
    int64_t numWrites;
    int64_t numRBytes;
    int64_t numWBytes;
    std::string domain;
    NetworkFlow() :
        opFlags(int32_t()),
        startTs(int64_t()),
        endTs(int64_t()),
        procOID(OID()),
        tid(int64_t()),
        sip(int32_t()),
        sport(int32_t()),
        dip(int32_t()),
        dport(int32_t()),
        proto(int32_t()),
        numReads(int64_t()),
        numWrites(int64_t()),
        numRBytes(int64_t()),
        numWBytes(int64_t()),
        domain(std::string())
        { }
};

}
namespace avro {
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

template<> struct codec_traits<sysflow.flow::NetworkFlow> {
    static void encode(Encoder& e, const sysflow.flow::NetworkFlow& v) {
        avro::encode(e, v.opFlags);
        avro::encode(e, v.startTs);
        avro::encode(e, v.endTs);
        avro::encode(e, v.procOID);
        avro::encode(e, v.tid);
        avro::encode(e, v.sip);
        avro::encode(e, v.sport);
        avro::encode(e, v.dip);
        avro::encode(e, v.dport);
        avro::encode(e, v.proto);
        avro::encode(e, v.numReads);
        avro::encode(e, v.numWrites);
        avro::encode(e, v.numRBytes);
        avro::encode(e, v.numWBytes);
        avro::encode(e, v.domain);
    }
    static void decode(Decoder& d, sysflow.flow::NetworkFlow& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.opFlags);
                    break;
                case 1:
                    avro::decode(d, v.startTs);
                    break;
                case 2:
                    avro::decode(d, v.endTs);
                    break;
                case 3:
                    avro::decode(d, v.procOID);
                    break;
                case 4:
                    avro::decode(d, v.tid);
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
                    avro::decode(d, v.numReads);
                    break;
                case 11:
                    avro::decode(d, v.numWrites);
                    break;
                case 12:
                    avro::decode(d, v.numRBytes);
                    break;
                case 13:
                    avro::decode(d, v.numWBytes);
                    break;
                case 14:
                    avro::decode(d, v.domain);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.opFlags);
            avro::decode(d, v.startTs);
            avro::decode(d, v.endTs);
            avro::decode(d, v.procOID);
            avro::decode(d, v.tid);
            avro::decode(d, v.sip);
            avro::decode(d, v.sport);
            avro::decode(d, v.dip);
            avro::decode(d, v.dport);
            avro::decode(d, v.proto);
            avro::decode(d, v.numReads);
            avro::decode(d, v.numWrites);
            avro::decode(d, v.numRBytes);
            avro::decode(d, v.numWBytes);
            avro::decode(d, v.domain);
        }
    }
};

}
#endif

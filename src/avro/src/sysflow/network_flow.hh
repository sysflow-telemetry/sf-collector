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
    int32_t fd;
    int64_t numRRecvOps;
    int64_t numWSendOps;
    int64_t numRRecvBytes;
    int64_t numWSendBytes;
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
        fd(int32_t()),
        numRRecvOps(int64_t()),
        numWSendOps(int64_t()),
        numRRecvBytes(int64_t()),
        numWSendBytes(int64_t())
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
        avro::encode(e, v.fd);
        avro::encode(e, v.numRRecvOps);
        avro::encode(e, v.numWSendOps);
        avro::encode(e, v.numRRecvBytes);
        avro::encode(e, v.numWSendBytes);
    }
    static void decode(Decoder& d, sysflow.flow::NetworkFlow& v) {
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
                    avro::decode(d, v.fd);
                    break;
                case 11:
                    avro::decode(d, v.numRRecvOps);
                    break;
                case 12:
                    avro::decode(d, v.numWSendOps);
                    break;
                case 13:
                    avro::decode(d, v.numRRecvBytes);
                    break;
                case 14:
                    avro::decode(d, v.numWSendBytes);
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
            avro::decode(d, v.fd);
            avro::decode(d, v.numRRecvOps);
            avro::decode(d, v.numWSendOps);
            avro::decode(d, v.numRRecvBytes);
            avro::decode(d, v.numWSendBytes);
        }
    }
};

}
#endif

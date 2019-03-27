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


#ifndef __SRC_SYSFLOW_FILE_FLOW_HH_4165041343__H_
#define __SRC_SYSFLOW_FILE_FLOW_HH_4165041343__H_


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

struct FOID {
    int64_t upperbits;
    int64_t lowerbits;
    FOID() :
        upperbits(int64_t()),
        lowerbits(int64_t())
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

template<> struct codec_traits<sysflow.flow::FOID> {
    static void encode(Encoder& e, const sysflow.flow::FOID& v) {
        avro::encode(e, v.upperbits);
        avro::encode(e, v.lowerbits);
    }
    static void decode(Decoder& d, sysflow.flow::FOID& v) {
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

template<> struct codec_traits<sysflow.flow::FileFlow> {
    static void encode(Encoder& e, const sysflow.flow::FileFlow& v) {
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
    static void decode(Decoder& d, sysflow.flow::FileFlow& v) {
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

}
#endif

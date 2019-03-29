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


#ifndef __SRC_SYSFLOW_CONTAINER_HH_2981602255__H_
#define __SRC_SYSFLOW_CONTAINER_HH_2981602255__H_


#include <sstream>
#include "boost/any.hpp"
#include "avro/Specific.hh"
#include "avro/Encoder.hh"
#include "avro/Decoder.hh"

namespace sysflow.entity {
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
    bool privileged;
    Container() :
        id(std::string()),
        name(std::string()),
        image(std::string()),
        imageid(std::string()),
        type(ContainerType()),
        privileged(bool())
        { }
};

}
namespace avro {
template<> struct codec_traits<sysflow.entity::ContainerType> {
    static void encode(Encoder& e, sysflow.entity::ContainerType v) {
		if (v < sysflow.entity::CT_DOCKER || v > sysflow.entity::CT_CUSTOM)
		{
			std::ostringstream error;
			error << "enum value " << v << " is out of bound for sysflow.entity::ContainerType and cannot be encoded";
			throw avro::Exception(error.str());
		}
        e.encodeEnum(v);
    }
    static void decode(Decoder& d, sysflow.entity::ContainerType& v) {
		size_t index = d.decodeEnum();
		if (index < sysflow.entity::CT_DOCKER || index > sysflow.entity::CT_CUSTOM)
		{
			std::ostringstream error;
			error << "enum value " << index << " is out of bound for sysflow.entity::ContainerType and cannot be decoded";
			throw avro::Exception(error.str());
		}
        v = static_cast<sysflow.entity::ContainerType>(index);
    }
};

template<> struct codec_traits<sysflow.entity::Container> {
    static void encode(Encoder& e, const sysflow.entity::Container& v) {
        avro::encode(e, v.id);
        avro::encode(e, v.name);
        avro::encode(e, v.image);
        avro::encode(e, v.imageid);
        avro::encode(e, v.type);
        avro::encode(e, v.privileged);
    }
    static void decode(Decoder& d, sysflow.entity::Container& v) {
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
                case 5:
                    avro::decode(d, v.privileged);
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
            avro::decode(d, v.privileged);
        }
    }
};

}
#endif

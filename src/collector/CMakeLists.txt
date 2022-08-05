#
# Copyright (C) 2022 IBM Corporation.
#
# Authors:
# Frederico Araujo <frederico.araujo@ibm.com>
# Teryl Taylor <terylt@ibm.com>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

include_directories("${CMAKE_SOURCE_DIR}/modules/sysflow/c++/")
set(
  COLLECTOR_SOURCES
  main.cpp
)

set(
  COLLECTOR_DEPENDENCIES
  sysflow
)

find_library(RT_LIB rt)

set(
  COLLECTOR_LIBRARIES
  sysflow
  sinsp
  "${GRPCPP_LIB}"
  "${GRPC_LIB}"
  "${GPR_LIB}"
  "${PROTOBUF_LIB}"
  "${CARES_LIB}"
  "${OPENSSL_LIBRARIES}"
  "${RT_LIB}"
  avrolib
  boost
  snappylib
)


set(
  COLLECTOR_INCLUDES
  "${PROJECT_BINARY_DIR}/src/libs/"
  "${CMAKE_SOURCE_DIR}/src/libs/"
)
add_executable(
  sysporter
  ${COLLECTOR_SOURCES}
)

add_dependencies(sysporter ${COLLECTOR_DEPENDENCIES})

target_include_directories(
  sysporter
  PUBLIC
  "${COLLECTOR_INCLUDES}"
)

target_link_directories(
 sysporter
 PUBLIC
 ${ELF_LIB_DIR}
)

target_link_libraries(
  sysporter
  ${COLLECTOR_LIBRARIES}
  "-static-libstdc++"
)

install(TARGETS sysporter RUNTIME DESTINATION ${SYSFLOW_BIN_DIR} COMPONENT "sysporter")

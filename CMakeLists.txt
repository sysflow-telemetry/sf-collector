# Copyright (C) 2022 IBM Corporation.
# Copyright (C) 2019 The Falco Authors.
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

# NOTE: This file is a modified version of the same file provided by the falco project in order to support compiling
# and integrating the falco libs into the project. Falco licensing is presented here:
# https://github.com/falcosecurity/falco/blob/master/COPYING

cmake_minimum_required(VERSION 3.5.1)

project(sfcollector)

option(USE_BUNDLED_DEPS "Bundle hard to find dependencies into the sfcollector binary" ON)
option(BUILD_WARNINGS_AS_ERRORS "Enable building with -Wextra -Werror flags" OFF)
option(MUSL_OPTIMIZED_BUILD "Enable if you want a musl optimized build" OFF)

# gVisor is currently only supported on Linux x86_64
if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" AND CMAKE_SYSTEM_NAME MATCHES "Linux" AND NOT MINIMAL_BUILD)
  option(BUILD_LIBS_GVISOR "Build gVisor support for sysflow libs" ON)
  if (BUILD_LIBS_GVISOR)
    add_definitions(-DHAS_GVISOR)
  endif()
endif()

# We shouldn't need to set this, see https://gitlab.kitware.com/cmake/cmake/-/issues/16419
option(EP_UPDATE_DISCONNECTED "ExternalProject update disconnected" OFF)
if (${EP_UPDATE_DISCONNECTED})
  set_property(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    PROPERTY EP_UPDATE_DISCONNECTED TRUE)
endif()

# Make flag for parallel processing
include(ProcessorCount)
processorcount(PROCESSOR_COUNT)
if(NOT PROCESSOR_COUNT EQUAL 0)
  set(PROCESSOUR_COUNT_MAKE_FLAG -j${PROCESSOR_COUNT})
endif()

# Custom CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

# GNU standard installation directories' definitions
include(GNUInstallDirs)

if(NOT SYSFLOW_DEBUG_FLAGS)
  set(SYSFLOW_DEBUG_FLAGS "-D_DEBUG")
endif()

string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE)
if(CMAKE_BUILD_TYPE STREQUAL "debug")
  set(KBUILD_FLAGS "${SYSFLOW_DEBUG_FLAGS} ${SYSFLOW_FEATURE_FLAGS}")
else()
  set(CMAKE_BUILD_TYPE "release")
  set(KBUILD_FLAGS "${SYSFLOW_FEATURE_FLAGS}")
  add_definitions(-DBUILD_TYPE_RELEASE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

if(MINIMAL_BUILD)
  set(MINIMAL_BUILD_FLAGS "-DMINIMAL_BUILD")
endif()

if(MUSL_OPTIMIZED_BUILD)
  set(MUSL_FLAGS "-static -Os -fPIE -pie")
  add_definitions(-DMUSL_OPTIMIZED)
endif()

# explicitly set hardening flags
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(SYSFLOW_SECURITY_FLAGS "-Wl,-z,relro,-z,now -fstack-protector-strong")
if(CMAKE_BUILD_TYPE STREQUAL "release")
  set(SYSFLOW_SECURITY_FLAGS "${SYSFLOW_SECURITY_FLAGS} -D_FORTIFY_SOURCE=2")
endif()

set(CMAKE_COMMON_FLAGS "${SYSFLOW_SECURITY_FLAGS} -Wall -ggdb ${SYSFLOW_FEATURE_FLAGS} ${MINIMAL_BUILD_FLAGS} ${MUSL_FLAGS}")

if(BUILD_WARNINGS_AS_ERRORS)
  set(CMAKE_SUPPRESSED_WARNINGS
      "-Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable -Wno-missing-field-initializers -Wno-sign-compare -Wno-type-limits -Wno-implicit-fallthrough -Wno-format-truncation -Wno-stringop-truncation -Wno-stringop-overflow -Wno-restrict"
  )
  set(CMAKE_COMMON_FLAGS "${CMAKE_COMMON_FLAGS} -Wextra -Werror ${CMAKE_SUPPRESSED_WARNINGS}")
endif()

set(CMAKE_C_FLAGS "${CMAKE_COMMON_FLAGS}")
set(CMAKE_CXX_FLAGS "--std=c++0x ${CMAKE_COMMON_FLAGS} -Wno-class-memaccess")

set(CMAKE_C_FLAGS_DEBUG "${SYSFLOW_DEBUG_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${SYSFLOW_DEBUG_FLAGS}")

set(CMAKE_C_FLAGS_RELEASE "-O3 -fno-strict-aliasing -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -fno-strict-aliasing -DNDEBUG")

#include(GetFalcoVersion)

set(PACKAGE_NAME "falco")
set(DRIVER_NAME "falco")
set(DRIVER_DEVICE_NAME "falco")
set(DRIVERS_REPO "https://download.falco.org/driver")

if(NOT DEFINED SYSFLOW_VERSION)
    set(SYSFLOW_VERSION "0.5.0")
endif()

if(NOT DEFINED SYSFLOW_BUILD_NUMBER)
    set(SYSFLOW_BUILD_NUMBER "1")
endif()

if(NOT DEFINED SYSFLOW_COMPONENT_NAME)
    set(SYSFLOW_COMPONENT_NAME "${CMAKE_PROJECT_NAME}")
endif()

if(NOT DEFINED FALCO_VERSION)
    set(FALCO_VERSION "0.32.1")
endif()

if(NOT FALCOSECURITY_LIBS_VERSION)
   set(FALCOSECURITY_LIBS_VERSION "0.7.0")
   set(FALCOSECURITY_LIBS_CHECKSUM "SHA256=3adc1620c0e830554a54cdd486158dc2c0c40552e113785b70fbbc99edb7d96f")
endif()


if(NOT DEFINED CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX "/usr/local/sysflow/")
endif()

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX
      /usr
      CACHE PATH "Default install path" FORCE)
endif()

set(CMD_MAKE make)
include(ExternalProject)
include(FindMakedev)

include(elfutils)
# libs
include(falcosecurity-libs)
# jq
include(jq)

# b64
include(b64)

# OpenSSL
include(openssl)

# libcurl
include(curl)


# One TBB
include(tbb)

include(zlib)
include(cares)
include(protobuf)
# gRPC
include(grpc)
include(snappy)
include(boost)
include(avro)
include(glog)
include(filesystem)
include(falco)
include(dkms)

# Shared build variables
set(FALCO_SINSP_LIBRARY sinsp)
set(SYSFLOW_BIN_DIR bin)

add_subdirectory(src/libs)
add_subdirectory(src/collector)

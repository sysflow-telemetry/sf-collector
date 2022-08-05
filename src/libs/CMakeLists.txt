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

include_directories(./)
include_directories("${CMAKE_SOURCE_DIR}/modules/sysflow/c++/")

set(SYSFLOW_SRC "${CMAKE_SOURCE_DIR}/src/libs/")
set(SYSFLOW_INCLUDE "")
list(APPEND SYSFLOW_INCLUDE
        "${SYSFLOW_SRC}"
	"${PROJECT_BINARY_DIR}/src/libs/"
    )

configure_file(sysflow_config.h.in sysflow_config.h)

include(ExternalProject)

set(SYSFLOW_SOURCES
	sysflowlibs.cpp 
	MurmurHash3.cpp
	utils.cpp
	containercontext.cpp
	processcontext.cpp
	processeventprocessor.cpp
	controlflowprocessor.cpp 
	dataflowprocessor.cpp
	networkflowprocessor.cpp
	fileflowprocessor.cpp
	fileeventprocessor.cpp
	sysflowcontext.cpp
	sysflowprocessor.cpp
	sysflowwriter.cpp
	sffilewriter.cpp
	sfsockwriter.cpp
	sfmultiwriter.cpp
	sfcallbackwriter.cpp
	filecontext.cpp
	k8scontext.cpp
	k8seventprocessor.cpp
)

add_library(sysflow STATIC ${SYSFLOW_SOURCES})

set(SYSFLOW_LIBRARIES
	sinsp
	"${AVRO_LIB}"
	"${ELF_LIB}"
	"${GLOG_LIB}"
	"${CARES_LIB}")

add_dependencies(sysflow sinsp avro snappy filesystem glog)
target_include_directories(sysflow PUBLIC "${LIBSINSP_INCLUDE_DIRS}")
target_link_libraries(sysflow ${SYSFLOW_LIBRARIES})
include_directories(${SYSFLOW_INCLUDE})
set(LIBSYSFLOW_LIB "${PROJECT_BINARY_DIR}/src/libs/libsysflow.a")
install(FILES "${LIBSYSFLOW_LIB}" DESTINATION "${CMAKE_INSTALL_LIBDIR}"
			COMPONENT "sysflow")
install(DIRECTORY "${SYSFLOW_SRC}" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
			COMPONENT "sysflow"
			FILES_MATCHING PATTERN "*.h")
install(DIRECTORY "${CMAKE_SOURCE_DIR}/modules/sysflow/c++/sysflow" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
			COMPONENT "sysflow"
			FILES_MATCHING PATTERN "*.h")
install(DIRECTORY "${PROJECT_BINARY_DIR}/src/libs/" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
			COMPONENT "sysflow"
			FILES_MATCHING PATTERN "*.h")

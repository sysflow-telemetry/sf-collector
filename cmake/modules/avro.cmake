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

set(AVRO_SRC "${PROJECT_BINARY_DIR}/avro-prefix/src/avro")
set(AVRO_INCLUDE "${AVRO_SRC}/target/include/")
set(AVRO_LIB "${AVRO_SRC}/target/lib/libavrocpp_s.a")
set(AVRO_INSTALL_DIR "${AVRO_SRC}/target")

ExternalProject_add(
	avro
	PREFIX "${PROJECT_BINARY_DIR}/avro-prefix"
	DEPENDS snappylib
	URL https://github.com/apache/avro/archive/refs/tags/release-1.11.0.tar.gz
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${AVRO_INSTALL_DIR}
	-DCMAKE_CXX_FLAGS_ALL_WARNINGS:STRING="-w" -DCMAKE_CXX_FLAGS="-I${SNAPPY_INCLUDE}"
	-DCMAKE_BUILD_TYPE=ALL_WARNINGS -DCMAKE_PREFIX_PATH="${SNAPPY_INSTALL_DIR}" -DSNAPPY_LIBRARIES=${SNAPPY_LIB} -DSNAPPY_INCLUDE_DIR=${SNAPPY_INCLUDE} -G "Unix Makefiles"
	SOURCE_SUBDIR  "lang/c++"
	BUILD_COMMAND "make"
	#BUILD_BYPRODUCTS ${AVRO_INCLUDE} ${AVRO_LIB}
        INSTALL_COMMAND make install
        )


add_library(avrolib SHARED IMPORTED)
set_target_properties(avrolib PROPERTIES IMPORTED_LOCATION ${AVRO_LIB})
include_directories("${AVRO_INCLUDE}")
install(FILES "${AVRO_LIB}" DESTINATION "${CMAKE_INSTALL_LIBDIR}"
			COMPONENT "avro")
install(DIRECTORY "${AVRO_INCLUDE}" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
			COMPONENT "avro"
			FILES_MATCHING PATTERN "*.h")

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

cmake_minimum_required(VERSION 3.5.1)
project(elfutils)
include(ExternalProject)
message(STATUS "Source include dir: ${CMAKE_CURRENT_SOURCE_DIR}")
ExternalProject_add(
	elfutils
	PREFIX "${ELF_PREFIX}"
	#DEPENDS boost
	GIT_REPOSITORY git://sourceware.org/git/elfutils.git
	GIT_TAG elfutils-0.179
	CONFIGURE_COMMAND autoreconf -i -f && ./configure --enable-maintainer-mode --disable-debuginfod --prefix=${ELF_INSTALL_DIR}
	BUILD_IN_SOURCE 1
	BUILD_BYPRODUCTS ${ELF_INCLUDE} ${ELF_LIB}
        INSTALL_COMMAND make install
        )
#include_directories("${ELF_SRC}/libelf/")
#include_directories("${ELF_INCLUDE}")

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

set(DKMS_SRC "${PROJECT_BINARY_DIR}/dkms-prefix/src/dkms")
set(DKMS_INSTALL_DIR "${DKMS_SRC}/target")


ExternalProject_add(
	dkms
	PREFIX "${PROJECT_BINARY_DIR}/dkms-prefix"
	URL https://github.com/dell/dkms/archive/refs/tags/v3.0.6.tar.gz
        CONFIGURE_COMMAND ""
	BUILD_COMMAND make SHAREDIR=${DKMS_INSTALL_DIR} DESTDIR=${DKMS_INSTALL_DIR} install-redhat
        INSTALL_COMMAND ""
	BUILD_IN_SOURCE 1
        )

install(DIRECTORY "${DKMS_INSTALL_DIR}" DESTINATION "${CMAKE_INSTALL_RUNSTATEDIR}"
			COMPONENT "dkms")

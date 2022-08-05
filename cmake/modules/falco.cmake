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

set(FALCO_SRC "${PROJECT_BINARY_DIR}/falco-prefix/src/falco")
set(FALCO_BUILD "${PROJECT_BINARY_DIR}/falco-prefix/src/falco-build")
set(FALCO_CMAKE "${CMAKE_SOURCE_DIR}/cmake/modules/falco-driver-loader.cmake")

ExternalProject_Add(
    falco
    PREFIX "falco-prefix"
    URL https://github.com/falcosecurity/falco/archive/refs/tags/0.32.1.tar.gz
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

set(FALCO_DRIVER_SRC_SCRIPT "${FALCO_SRC}/scripts/falco-driver-loader")
set(FALCO_DOCKER_ENTRY_POINT "${FALCO_SRC}/docker/falco/docker-entrypoint.sh")
set(FALCO_DRIVER_OUT_SCRIPT "${FALCO_BUILD}/falco-driver-loader")

add_custom_command(
             TARGET falco POST_BUILD
             COMMAND ${CMAKE_COMMAND} -DINPUT_SCR_FILE=${FALCO_DRIVER_SRC_SCRIPT} -DOUTPUT_SCR_FILE=${FALCO_DRIVER_OUT_SCRIPT} -DDRIVERS_REPO=${DRIVERS_REPO} -DDRIVER_VERSION=${DRIVER_VERSION} -DDRIVER_NAME=${DRIVER_NAME} -DFALCO_VERSION=${FALCO_VERSION} -P ${FALCO_CMAKE}
             VERBATIM
             )

install(FILES "${FALCO_DRIVER_OUT_SCRIPT}" DESTINATION "${CMAKE_INSTALL_BINDIR}"
			COMPONENT "falco")
install(FILES "${FALCO_DOCKER_ENTRY_POINT}" DESTINATION "${CMAKE_INSTALL_BINDIR}"
			COMPONENT "falco")

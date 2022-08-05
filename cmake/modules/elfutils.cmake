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

set(ELF_CMAKE_WORKING_DIR "${CMAKE_BINARY_DIR}/elfutils-repo")
set(ELF_CMAKE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/elfutils")

set(ELF_SRC "${PROJECT_BINARY_DIR}/elfutils-prefix/src/elfutils")
set(ELF_PREFIX "${PROJECT_BINARY_DIR}/elfutils-prefix")
set(ELF_INCLUDE "${ELF_SRC}/target/include/")
set(ELF_LIB "${ELF_SRC}/target/lib/libelf.a")
set(ELF_INSTALL_DIR "${ELF_SRC}/target")
set(ELF_LIB_DIR "${ELF_INSTALL_DIR}/lib/")
message(STATUS "Source include dir: ${CMAKE_CURRENT_SOURCE_DIR}")

file(MAKE_DIRECTORY ${ELF_CMAKE_WORKING_DIR})
# Configure external project
execute_process(
    COMMAND ${CMAKE_COMMAND} -DELF_PREFIX=${ELF_PREFIX} -DELF_INCLUDE=${ELF_INCLUDE} -DELF_LIB=${ELF_LIB} -DELF_INSTALL_DIR=${ELF_INSTALL_DIR} "${ELF_CMAKE_SOURCE_DIR}"
    WORKING_DIRECTORY ${ELF_CMAKE_WORKING_DIR}
)

# Build external project
execute_process(
    COMMAND ${CMAKE_COMMAND} --build .
    WORKING_DIRECTORY ${ELF_CMAKE_WORKING_DIR}
)
include_directories("${ELF_INCLUDE}")
install(FILES "${ELF_LIB}" DESTINATION "${CMAKE_INSTALL_LIBDIR}"
			COMPONENT "elfutils")
install(DIRECTORY "${ELF_INCLUDE}" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
			COMPONENT "elfutils"
			FILES_MATCHING PATTERN "*.h")

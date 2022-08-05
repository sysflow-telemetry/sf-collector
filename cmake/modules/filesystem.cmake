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

set(FS_SRC "${PROJECT_BINARY_DIR}/filesystem-prefix/src/filesystem")
set(FS_INCLUDE "${FS_SRC}/target/include/")
set(FS_INSTALL_DIR "${FS_SRC}/target")

ExternalProject_add(
	filesystem
	PREFIX "${PROJECT_BINARY_DIR}/filesystem-prefix"
	URL https://github.com/gulrak/filesystem/archive/refs/tags/v1.5.12.tar.gz
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${FS_INSTALL_DIR}
	BUILD_BYPRODUCTS ${FS_INCLUDE}
        INSTALL_COMMAND make install
        )

include_directories("${FS_INCLUDE}")
install(DIRECTORY "${FS_INCLUDE}" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
			COMPONENT "filesystem"
			FILES_MATCHING PATTERN "*.h")

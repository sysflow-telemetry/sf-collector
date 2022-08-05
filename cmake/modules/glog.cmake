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

set(GLOG_SRC "${PROJECT_BINARY_DIR}/glog-prefix/src/glog")
set(GLOG_INCLUDE "${GLOG_SRC}/target/include/")
set(GLOG_LIB "${GLOG_SRC}/target/${CMAKE_INSTALL_LIBDIR}/libglog.a")
set(GLOG_INSTALL_DIR "${GLOG_SRC}/target")


ExternalProject_add(
	glog
	PREFIX "${PROJECT_BINARY_DIR}/glog-prefix"
	#DEPENDS boost
	URL https://github.com/google/glog/archive/refs/tags/v0.6.0.tar.gz
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${GLOG_INSTALL_DIR}
	-S . -G "Unix Makefiles" -DBUILD_SHARED_LIBS=OFF -DBUILD_TESTING=OFF -DWITH_GFLAGS=OFF
	#BUILD_COMMAND "cd build && make"
	BUILD_BYPRODUCTS ${GLOG_INCLUDE} ${GLOG_LIB}
        INSTALL_COMMAND make install
        )

include_directories("${GLOG_INCLUDE}")
install(FILES "${GLOG_LIB}" DESTINATION "${CMAKE_INSTALL_LIBDIR}"
			COMPONENT "glog")
install(DIRECTORY "${GLOG_INCLUDE}" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
			COMPONENT "glog"
			FILES_MATCHING PATTERN "*.h")

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

set(SNAPPY_SRC "${PROJECT_BINARY_DIR}/snappy-prefix/src/snappy")
set(SNAPPY_INCLUDE "${SNAPPY_SRC}/target/include/")
set(SNAPPY_LIB "${SNAPPY_SRC}/target/${CMAKE_INSTALL_LIBDIR}/libsnappy.a")
set(SNAPPY_INSTALL_DIR "${SNAPPY_SRC}/target")

ExternalProject_add(
	snappy
	PREFIX "${PROJECT_BINARY_DIR}/snappy-prefix"
	#DEPENDS boost
	URL https://github.com/google/snappy/archive/refs/tags/1.1.9.tar.gz
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${SNAPPY_INSTALL_DIR} -DCMAKE_CXX_FLAGS="-fPIC"
	-S . -G "Unix Makefiles" -DBUILD_SHARED_LIBS=OFF -DSNAPPY_BUILD_BENCHMARKS=OFF -DSNAPPY_BUILD_TESTS=OFF
	#BINARY_DIR "build"
	#BUILD_COMMAND "cd build && make"
	BUILD_BYPRODUCTS ${SNAPPY_INCLUDE} ${SNAPPY_LIB}
        INSTALL_COMMAND make install
	)
add_library(snappylib SHARED IMPORTED)
set_target_properties(snappylib PROPERTIES IMPORTED_LOCATION ${SNAPPY_LIB})
include_directories("${SNAPPY_INCLUDE}")
install(FILES "${SNAPPY_LIB}" DESTINATION "${CMAKE_INSTALL_LIBDIR}"
			COMPONENT "snappylib")
install(DIRECTORY "${SNAPPY_INCLUDE}" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
			COMPONENT "snappylib"
			FILES_MATCHING PATTERN "*.h")
